#pragma once

#include <sys/types.h>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <vector>
#include <memory>

class SocketCommand
{
public:
    SocketCommand(uint8_t commandId, uint8_t payloadLen, const uint8_t* payloadPtr);
    ~SocketCommand();
    uint8_t GetCommandId() const { return m_commandId; }
    uint8_t GetLength() const { return m_payloadLen; }
    const uint8_t* GetBuffer() const { return m_payloadBuf; }
    void Ack(uint8_t payloadLen, const uint8_t* payloadPtr);
private:
    uint8_t m_commandId;
    uint8_t m_payloadLen;
    uint8_t* m_payloadBuf;   
};

class SocketConnection;

class SocketServer
{
public:
    SocketServer();
    ~SocketServer();

    SocketServer(SocketServer const&) = delete;
    SocketServer& operator =(SocketServer const&) = delete;

    void BeginListening();
    void StopListening();
    void SetDispatcherCallback(std::function<void(const SocketCommand&)> func);
private:
    void RunListener();
    void ConnectionDataCallback(const SocketCommand&);
    void SetupSocket();
    void CloseSocket();
    void AcceptConnection();
    void RemoveConnection(SocketConnection* connPtr);

    std::vector<std::unique_ptr<SocketConnection>> m_activeConnections;
    std::thread m_listenerThread;
    std::mutex m_listenerLock;
    bool m_isRunning;
    bool m_inFinalRelease;
    int m_sockfd;
    
    const int c_port = 5001;
    const int c_backlog = 5;
};

class SocketConnection
{
public:
    SocketConnection(
        int sockfd, 
        std::function<void(const SocketCommand&)> dataCallback);
    ~SocketConnection();
    void SetDeletionCallback(std::function<void()> deletionCallback);
private:
    void RunReceiver();
    int m_sockfd;
    std::function<void(const SocketCommand&)> m_dataCallback;
    std::function<void()> m_deletionCallback;
    std::thread m_receiverThread;

    const uint32_t c_bufferSize = 256;
};
