#include "LightsCore.h"
#include "SocketServer.h"

void LightsCore::Run()
{
    std::cout << "Initializing main routine..." << std::endl;
    m_socketServer.BeginListening();
    // Initialize the PatternRunner, SocketServer
    // Start the PatternRunner thread
}

