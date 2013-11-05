#pragma once

#include <stdint.h>
#include <cstdio>

class Driver
{
private:
    static FILE* pFile; 

public:
    static void OpenDevice();
    static void CloseDevice();
    static void InitializeBulbs();
    static void SetBulb(uint8_t bulbIdx, uint8_t brightness, uint8_t red, uint8_t green, uint8_t blue);
};
