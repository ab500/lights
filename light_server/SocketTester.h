#pragma once

#include <map>
#include <functional>

#include "ISocketCommandConsumer.h"

class SocketCommand;

class SocketTester:
    public ISocketCommandConsumer
{
public:
    virtual void RegisterCallbacks(
        std::map<
            unsigned int, 
            std::function<void(const SocketCommand&)>>& commandMap);
    virtual void UnregisterCallbacks(
        std::map<
            unsigned int, 
            std::function<void(const SocketCommand&)>> & commandMap); 
private:
    void PrintCallback(const SocketCommand& command);
};
