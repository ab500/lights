#pragma once

#include <stdint.h>

class Driver
{
private:
    static int fd;

public:
    static void OpenDevice();
    static void CloseDevice();
    static void SetBulb(uint8_t bulbIdx, uint8_t brightness, uint8_t red, uint8_t green, uint8_t blue);
};
