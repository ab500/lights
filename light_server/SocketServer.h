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
    SocketCommand(uint8_t commandId, uint32_t payloadLen, const uint8_t* payloadPtr, std::function<void(uint32_t, const uint8_t*)> sendCallback);
    ~SocketCommand();
    uint8_t GetCommandId() const { return m_commandId; }
    uint32_t GetLength() const { return m_payloadLen; }
    const uint8_t* GetBuffer() const { return m_payloadBuf; }
    void Ack(uint32_t payloadLen, const uint8_t* payloadPtr) const;
private:
    const uint8_t m_commandId;
    const uint32_t m_payloadLen;
    const uint8_t* m_payloadBuf;   
    const std::function<void(uint32_t, const uint8_t*)> m_sendCallback;
};

class SocketConnection;

class SocketServer
{
public:
    SocketServer(std::function<void(const SocketCommand&)> dispatcherCallback);
    ~SocketServer();

    SocketServer(SocketServer const&) = delete;
    SocketServer& operator =(SocketServer const&) = delete;

    void BeginListening();
    void StopListening();

private:
    void RunListener();
    void ConnectionDataCallback(const SocketCommand& cmd);
    void SetupSocket();
    void CloseSocket();
    void AcceptConnection();
    void RemoveConnection(SocketConnection* connPtr);

    std::function<void(const SocketCommand&)> m_dispatcherCallback;
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

    SocketConnection(SocketServer const&) = delete;
    SocketConnection& operator =(SocketServer const&) = delete;

private:
    void RunReceiver();
    void SendData(uint32_t payloadLen, const uint8_t* payloadPtr);

    int m_sockfd;
    std::function<void(const SocketCommand&)> m_dataCallback;
    std::function<void()> m_deletionCallback;
    std::thread m_receiverThread;

    const uint32_t c_bufferSize = 256;
};
