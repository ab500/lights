#pragma once

#include <stdint.h>
#include <vector>
#include <utility>


struct BulbCommand
{
    uint8_t brightness;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

class LightBoard
{
    void SetBulb(int bulbIdx, BulbCommand cmd);
    void GetUpdate(std::vector<std::pair<int, BulbCommand>> & deltaList);
    void Reset(bool generateClearCommands);
};
