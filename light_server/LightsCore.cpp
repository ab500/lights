#include "LightsCore.h"
#include "SocketServer.h"

LightsCore::LightsCore()
    : m_socketServer(std::bind(&LightsCore::DispatcherCallback, this, std::placeholders::_1))
{
    m_socketTester.RegisterCallbacks(m_commandMap);
    m_patternRunner.RegisterCallbacks(m_commandMap);
}

LightsCore::~LightsCore()
{

}

void LightsCore::DispatcherCallback(const SocketCommand& command)
{
    const int commandId = command.GetCommandId();
    
    if (m_commandMap.count(commandId) > 0) {
        m_commandMap[commandId](command);
    }
    else {
        std::cout << "Command id not recognized." << std::endl;
    }
}

void LightsCore::Run()
{
    std::cout << "Initializing main routine..." << std::endl;
    m_socketServer.BeginListening();
    // Initialize the PatternRunner, SocketServer
    // Start the PatternRunner thread
}

