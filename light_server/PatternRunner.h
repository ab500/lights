#pragma once

#include <map>
#include <functional>
#include <stdint.h>
#include <thread>

#include "Driver.h"
#include "LightBoard.h"
#include "ISocketCommandConsumer.h"
#include "IPattern.h"

class PatternRunner:
    public ISocketCommandConsumer
{
public:
    PatternRunner();
    ~PatternRunner();

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
    void SetPattern(IPattern* pPattern);

private:
    LightBoard m_board;
    IPattern* m_pCurrentPattern;

    std::thread m_tickThread;
    bool m_isRunning;
    uint8_t m_brightness;
    uint8_t m_hue;
    uint8_t m_saturation;
    uint8_t m_nightMode;
    bool m_pendingReset;

    void ReadSettingsCallback(const SocketCommand& command);
    void WriteSettingsCallback(const SocketCommand& command);
    void ResetDeviceCallback(const SocketCommand& command);
    void Run();

    void ApplyBrightness(BulbCommand & cmd);
    void ApplySaturation(BulbCommand & cmd);
    void ApplyHue(BulbCommand & cmd);
};
