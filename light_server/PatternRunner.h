#pragma once

#include <map>
#include <functional>
#include <stdint.h>

#include "ISocketCommandConsumer.h"

class PatternRunner:
    public ISocketCommandConsumer
{
public:
    PatternRunner();

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
    uint8_t m_brightness;
    uint8_t m_hue;
    uint8_t m_saturation;
    uint8_t m_nightMode;

    void ReadSettingsCallback(const SocketCommand& command);
    void WriteSettingsCallback(const SocketCommand& command);
    void ResetDeviceCallback(const SocketCommand& command);
};
