#pragma once

#include <sys/types.h>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>

class SocketCommand
{
public:
    SocketCommand(uint8_t commandId, uint8_t payloadLen, const uint8_t* payloadPtr);
    ~SocketCommand();
    uint8_t GetCommandId() const { return m_commandId; }
    uint8_t GetLength() const { return m_payloadLen; }
    const uint8_t* GetBuffer() const { return m_payloadBuf; }
private:
    uint8_t m_commandId;
    uint8_t m_payloadLen;
    uint8_t* m_payloadBuf;   
};

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
    void AttachDebugPrinter(std::function<void(const std::string&)> func);
private:
    void RunListener();
    std::thread m_listenerThread;
    std::mutex m_listenerLock;
    std::function<void(const std::string&)> m_debugPrinter;
    bool m_isRunning;
};

