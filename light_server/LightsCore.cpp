#include "LightsCore.h"
#include "SocketServer.h"

LightsCore::LightsCore()
    : m_socketServer(std::bind(&LightsCore::DispatcherCallback, this, std::placeholders::_1))
{
}

LightsCore::~LightsCore()
{

}

void LightsCore::DispatcherCallback(const SocketCommand& command)
{

}

void LightsCore::Run()
{
    std::cout << "Initializing main routine..." << std::endl;
    m_socketServer.BeginListening();
    // Initialize the PatternRunner, SocketServer
    // Start the PatternRunner thread
}

