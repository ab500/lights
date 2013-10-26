#pragma once

#include <map>
#include <functional>

#include "ISocketCommandConsumer.h"

class PatternRunner:
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

    void Start();
    void Stop();

private:
    void ReadSettingsCallback(const SocketCommand& command);
    void WriteSettingsCallback(const SocketCommand& command);
    void ResetDeviceCallback(const SocketCommand& command);
};
