#pragma once

#include <vector>
#include <utility>

#include <stdint.h>

struct BulbCommand
{
    uint8_t brightness;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

class LightBoard
{
public:
    static const int BulbCount = 100;
    void SetBulb(int bulbIdx, BulbCommand cmd);
    void GetUpdate(std::vector<std::pair<int, BulbCommand>> & deltaList);
    void Reset(bool generateClearCommands);

private:
    std::vector<std::pair<int, BulbCommand>> m_pendingUpdates;
};
