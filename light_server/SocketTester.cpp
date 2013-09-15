#include "SocketTester.h"

void SocketTester::RegisterCallbacks(
    std::map<
        unsigned int, 
        std::function<void(const SocketCommand&)>>& commandMap)
{

}

void SocketTester::UnregisterCallbacks(
    std::map<
        unsigned int, 
        std::function<void(const SocketCommand&)>> & commandMap)
{

}

void SocketTester::PrintCallback(const SocketCommand& command)
{

}

