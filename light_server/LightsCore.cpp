#include "LightsCore.h"
#include "SocketServer.h"

void LightsCore::Run()
{
    if (m_debugPrinter)
    {
        m_debugPrinter(std::string("this is a test"));
    }
    m_socketServer.BeginListening();
    // Initialize the PatternRunner, SocketServer
    // Start the PatternRunner thread
}

void LightsCore::AttachDebugPrinter(std::function<void(const std::string&)> printFunc)
{
    m_debugPrinter = printFunc;
    m_socketServer.AttachDebugPrinter(printFunc);
}
