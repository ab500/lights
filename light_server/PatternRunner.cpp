#include "PatternRunner.h"

#include <stdexcept>
#include <iostream>

#include "CallbackDefinitions.h"
#include "SocketServer.h"

PatternRunner::PatternRunner()
    : m_pCurrentPattern(nullptr)
    , m_isRunning(false)
    , m_brightness(200)
    , m_hue(128)
    , m_saturation(255)
    , m_nightMode(0)
    , m_pendingReset(false)
{
}

PatternRunner::~PatternRunner()
{
    Stop();
}

void PatternRunner::RegisterCallbacks(
    std::map<
        unsigned int,
        std::function<void(const SocketCommand&)>>& commandMap)
{
    if (commandMap.count(CallbackDefinitions::PatternRunner::ReadSettings) > 0 ||
        commandMap.count(CallbackDefinitions::PatternRunner::WriteSettings) > 0 ||
        commandMap.count(CallbackDefinitions::PatternRunner::ResetDevice) > 0) {
       throw std::runtime_error("Callback already registered."); 
    }

    commandMap[CallbackDefinitions::PatternRunner::ReadSettings] =
        std::bind(&PatternRunner::ReadSettingsCallback,
            this, std::placeholders::_1);
    commandMap[CallbackDefinitions::PatternRunner::WriteSettings] =
        std::bind(&PatternRunner::WriteSettingsCallback,
            this, std::placeholders::_1);
    commandMap[CallbackDefinitions::PatternRunner::ResetDevice] =
        std::bind(&PatternRunner::ResetDeviceCallback,
            this, std::placeholders::_1);
}

void PatternRunner::UnregisterCallbacks(
    std::map<
        unsigned int,
        std::function<void(const SocketCommand&)>>& commandMap)
{
    commandMap.erase(CallbackDefinitions::PatternRunner::ReadSettings);
    commandMap.erase(CallbackDefinitions::PatternRunner::WriteSettings);
    commandMap.erase(CallbackDefinitions::PatternRunner::ResetDevice);
}

void PatternRunner::ReadSettingsCallback(const SocketCommand& command)
{
    std::cout << "ReadSettings command received." << std::endl;
    uint8_t response[4] = {};

    response[0] = m_brightness;
    response[1] = m_hue;
    response[2] = m_saturation;
    response[3] = m_nightMode;

    std::cout << "ReadSettings: B: " << 
        (int)m_brightness << " H: " << 
        (int)m_hue << " S: " << 
        (int)m_saturation << " NM: " << 
        (int)m_nightMode << std::endl;
    command.Ack(sizeof(response), response);
}

void PatternRunner::WriteSettingsCallback(const SocketCommand& command)
{
    std::cout << "WriteSettings command received." << std::endl;
    
    if (command.GetLength() != 4) {
        std::cout << "Error. WriteSettings length incorrect." << std::endl;
    }

    const uint8_t* request = command.GetBuffer();

    m_brightness = request[0];
    m_hue = request[1];
    m_saturation = request[2];

    if (m_nightMode != request[3]) {
        m_pendingReset = true;
    }
    m_nightMode = request[3];

    std::cout << "WriteSettings: B: " << 
        (int)m_brightness << " H: " << 
        (int)m_hue << " S: " << 
        (int)m_saturation << " NM: " << 
        (int)m_nightMode << std::endl;
    command.Ack(0, nullptr);
}

void PatternRunner::ResetDeviceCallback(const SocketCommand& command)
{
    std::cout << "ResetDevice command received." << std::endl;
}

void PatternRunner::Start()
{
    if (!m_isRunning) {
        m_isRunning = true;
        m_tickThread = std::thread(&PatternRunner::Run, this);
    }
}

void PatternRunner::Stop()
{
    if (m_isRunning) {
        m_isRunning = false;
        m_tickThread.join();
    }
}

void PatternRunner::Run()
{
    while (m_isRunning) {
        std::vector<std::pair<int, BulbCommand>> cmds;

        if (m_pendingReset) {
            m_board.Reset(true);
            m_pendingReset = false;
        }
        else if (!m_nightMode) {
            if (m_pCurrentPattern) {
                m_pCurrentPattern->Tick(m_board);
            }
        }

        m_board.GetUpdate(cmds);

        for (auto iter = cmds.begin(); iter != cmds.end(); iter++) {
            ApplyBrightness((*iter).second);
            ApplySaturation((*iter).second);
            ApplyHue((*iter).second); 
            Driver::SetBulb(static_cast<uint8_t>((*iter).first),
                (*iter).second.brightness,
                (*iter).second.red,
                (*iter).second.green,
                (*iter).second.blue);
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    } 
}

void PatternRunner::SetPattern(IPattern* pPattern)
{
    m_pCurrentPattern = pPattern;
}

void PatternRunner::ApplyBrightness(BulbCommand& cmd)
{
    cmd.brightness = static_cast<uint8_t>(
        cmd.brightness * (static_cast<double>(m_brightness) / 255));
}

void PatternRunner::ApplySaturation(BulbCommand& cmd)
{

}

void PatternRunner::ApplyHue(BulbCommand& cmd)
{

}


