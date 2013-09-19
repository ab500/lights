#pragma once

#include <map>
#include <functional>

class SocketCommand;

class ISocketCommandConsumer
{
public:
    virtual ~ISocketCommandConsumer() {};
    virtual void RegisterCallbacks(
        std::map<
            unsigned int, 
            std::function<void(const SocketCommand&)>>& commandMap) = 0;
    virtual void UnregisterCallbacks(
        std::map<
            unsigned int, 
            std::function<void(const SocketCommand&)>>& commandMap) = 0; 
};
