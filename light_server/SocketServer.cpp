#include "SocketServer.h"

SocketCommand::SocketCommand(uint8_t commandId, uint8_t payloadLen, const uint8_t* payloadPtr)
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

SocketServer::SocketServer()
    : m_isRunning(false)
{
}

SocketServer::~SocketServer()
{
    StopListening();
    // Clear the queue of accetped connections.
}

void SocketServer::BeginListening()
{
    std::lock_guard<std::mutex> lock(m_listenerLock);

    if (!m_isRunning) {
        m_listenerThread = std::thread(&SocketServer::RunListener, this);
        m_isRunning = true;
    }   
}

void SocketServer::StopListening()
{
    std::lock_guard<std::mutex> lock(m_listenerLock);

    if (m_isRunning) {
        m_isRunning = false;
        m_listenerThread.join();
    }
}

void SocketServer::SetDispatcherCallback(std::function<void(const SocketCommand&)> func)
{
}

void SocketServer::AttachDebugPrinter(std::function<void(const std::string&)> func)
{
    std::lock_guard<std::mutex> lock(m_listenerLock);
    m_debugPrinter = func;
}

void SocketServer::RunListener()
{
    m_debugPrinter("From another thread, this time with feeling!");
}
