#include "SocketServer.h"

#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdexcept>
#include <algorithm>

SocketCommand::SocketCommand(uint8_t commandId, uint32_t payloadLen, const uint8_t* payloadPtr)
    : m_commandId(commandId)
    , m_payloadLen(payloadLen)
    , m_payloadBuf(nullptr)
{
    m_payloadBuf = new uint8_t[payloadLen];
}

SocketCommand::~SocketCommand()
{
    if (m_payloadBuf) {
        delete[] m_payloadBuf;
        m_payloadBuf = nullptr;
    }
}

SocketServer::SocketServer(std::function<void(const SocketCommand&)> dispatcherCallback)
    : m_dispatcherCallback(dispatcherCallback)
    , m_isRunning(false)
    , m_inFinalRelease(true)
    , m_sockfd(-1)
{
}

SocketServer::~SocketServer()
{
    m_inFinalRelease = true;
    StopListening();
}

void SocketServer::BeginListening()
{
    std::lock_guard<std::mutex> lock(m_listenerLock);

    if (!m_isRunning) {
        m_isRunning = true;
        m_listenerThread = std::thread(&SocketServer::RunListener, this);
    }   
}

void SocketServer::StopListening()
{
    std::cout << "Stopping..." << std::endl;
    std::lock_guard<std::mutex> lock(m_listenerLock);
    CloseSocket();
    if (m_isRunning) {
        m_isRunning = false;
        m_listenerThread.join();
    }
}

void SocketServer::RunListener()
{
    std::cout << "Initializing server on port " << c_port << std::endl;
    SetupSocket();
    
    std::cout << "Waiting for connections..." << std::endl;
    while (m_isRunning)
    {
        AcceptConnection();
    }

    std::cout << "Socket closed by SocketServer." << std::endl;
}

void SocketServer::SetupSocket()
{
    int status = 0;
    int sockfd = -1;
    struct addrinfo hints = {};
    struct addrinfo* servInfo = nullptr;
    char portStr[10] = {};

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    sprintf(portStr, "%d", c_port);

    status = getaddrinfo(NULL, portStr, &hints, &servInfo);
    if (status < 0) {
        throw std::runtime_error("Failed to get address info.");
    }
   
    sockfd = socket(servInfo->ai_family, servInfo->ai_socktype, servInfo->ai_protocol);
    if (sockfd == -1) {
        throw std::runtime_error("Failed to create socket.");
    }
    
    status = bind(sockfd, servInfo->ai_addr, servInfo->ai_addrlen);
    if (status < 0) {
        close(sockfd);
        throw std::runtime_error("Failed to bind to port.");
    }

    freeaddrinfo(servInfo);
    
    status = listen(sockfd, c_backlog);
    if (status < 0) {
        close(sockfd);
        throw std::runtime_error("Failed to begin listening");
    }
    
    m_sockfd = sockfd;
}

void SocketServer::CloseSocket()
{
    shutdown(m_sockfd, SHUT_RDWR);
    close(m_sockfd);
    m_sockfd = -1;
}

void SocketServer::AcceptConnection()
{
    struct sockaddr_storage theirAddr = {};
    socklen_t sinSize = sizeof theirAddr;
    int newFd = accept(m_sockfd, (struct sockaddr *)&theirAddr, &sinSize);
    
    if (newFd != -1) {
        std::cout << "Accepting a new connection." << std::endl;
        std::lock_guard<std::mutex> lock(m_listenerLock);

        std::unique_ptr<SocketConnection> newConPtr = std::unique_ptr<SocketConnection>(
            new SocketConnection(
                newFd,
                std::bind(&SocketServer::ConnectionDataCallback, this, std::placeholders::_1)));
        newConPtr->SetDeletionCallback(
            std::bind(&SocketServer::RemoveConnection, this, newConPtr.get()));
        m_activeConnections.push_back(std::move(newConPtr));
    }
    else {
        std::cout << "No valid connection accepted." << std::endl;
    }
}

void SocketServer::RemoveConnection(SocketConnection* connPtr)
{
    std::cout << "Removing socket ID " << connPtr << std::endl;
    std::lock_guard<std::mutex> lock(m_listenerLock);
    if (!m_inFinalRelease) {
        m_activeConnections.erase(
            std::remove_if(
                m_activeConnections.begin(),
                m_activeConnections.end(),
                [connPtr](std::unique_ptr<SocketConnection>& item) 
                { return item.get() == connPtr; })); 
    }
}

void SocketServer::ConnectionDataCallback(const SocketCommand& cmd)
{
   m_dispatcherCallback(cmd); 
}

SocketConnection::SocketConnection(
    int sockfd,
    std::function<void(const SocketCommand&)> dataCallback)
    : m_sockfd(sockfd)
    , m_dataCallback(dataCallback)
{
    m_receiverThread = std::thread(&SocketConnection::RunReceiver, this);
}

SocketConnection::~SocketConnection()
{
    if (m_sockfd != -1) {
        shutdown(m_sockfd, SHUT_RDWR);
        close(m_sockfd);
    }
    
    // Lifetime management of this object is a little tricky. There's
    // two ways this object can fall out of existence:
    // 1) When the socket is closed by the client it will invoke from
    //    the listening thread the deletion callback to have SocketServer
    //    destruct it.
    // 2) When SocketServer itself is being destructed (application shutdown)
    //    we actually shutdown the listener thread from the other thread and join.
    //
    // Either way, at the end of this function the object can safely be destructed
    // and the listening thread is dead.
    if (std::this_thread::get_id() != m_receiverThread.get_id()) {
        m_receiverThread.join();
    }
}

void SocketConnection::SetDeletionCallback(std::function<void()> deletionCallback)
{
    m_deletionCallback = deletionCallback;
}

void SocketConnection::RunReceiver()
{
    while (true) {
        uint32_t bytesToRead = 0;
        bool succeeded = false;

        ssize_t byteLen = recv(m_sockfd, static_cast<void*>(&bytesToRead), sizeof(uint32_t), MSG_WAITALL);
        std::cout << "Received " << byteLen << " bytes. Payload is " << bytesToRead << " bytes." << std::endl;

        if (byteLen == 4 && bytesToRead <= c_bufferSize) {
            uint8_t payload[byteLen];
            std::fill(payload, payload+byteLen, 0);

            ssize_t bytesRead = recv(m_sockfd, static_cast<void*>(payload), byteLen, MSG_WAITALL);

            if (bytesRead == bytesToRead) {
                succeeded = true;
                
                if (m_dataCallback) {
                    SocketCommand sockCmd = SocketCommand(payload[0], byteLen - 1, payload + 1); 
                    // Fire off a callback into the dark
                    m_dataCallback(sockCmd);
                }
            }
            else {
                std::cout << "Byte length is only.... " << bytesRead << std::endl;
            }
        }

        if (!succeeded) {
            std::cout << "Closed / Invalid data received from socket " << m_sockfd << 
                ". Aborting connection." << std::endl;

            if (m_deletionCallback) {
                m_deletionCallback();
                // The object may be destructed after this point. Do not access any
                // member state.
            }
            break;
        }
    }
}
