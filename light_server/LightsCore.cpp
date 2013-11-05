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
    Driver::CloseDevice();
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
    Driver::OpenDevice();
    Driver::InitializeBulbs();

    // Green bulb!
    Driver::SetBulb(0, 200, 0, 13, 0);

    std::cout << "Initializing main routine..." << std::endl;
    m_patternRunner.Start();
    m_patternRunner.SetPattern(&m_rainbowPattern);
    m_socketServer.BeginListening();
}

