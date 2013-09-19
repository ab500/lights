#include "SocketTester.h"

#include <stdexcept>
#include <iostream>

#include "CallbackDefinitions.h"
#include "SocketServer.h"

void SocketTester::RegisterCallbacks(
    std::map<
        unsigned int, 
        std::function<void(const SocketCommand&)>>& commandMap)
{
    if (commandMap.count(CallbackDefinitions::SocketTester::Print)
        > 0)
    {
        throw std::runtime_error("Callback already registered."); 
    }

    commandMap[CallbackDefinitions::SocketTester::Print] =
        std::bind(&SocketTester::PrintCallback, 
            this, std::placeholders::_1);
}

void SocketTester::UnregisterCallbacks(
    std::map<
        unsigned int, 
        std::function<void(const SocketCommand&)>> & commandMap)
{
    commandMap.erase(CallbackDefinitions::SocketTester::Print);
}

// Simple echo server!
void SocketTester::PrintCallback(const SocketCommand& command)
{
    uint32_t length = command.GetLength();
    uint8_t strBuff[length + 1];
    std::copy(command.GetBuffer(), command.GetBuffer() + length, strBuff);
    strBuff[length] = 0;     
    std::cout << "Data received: " << (char*)(strBuff) << std::endl;
    command.Ack(length, command.GetBuffer());
    std::cout << "Sent ack." << std::endl;
}

