#include "Driver.h"

#include <stdexcept>

FILE* Driver::pFile = nullptr;

void Driver::OpenDevice()
{
    if (pFile) {
        throw std::runtime_error("Device file already open.");
    }

    pFile = fopen("/dev/xmas", "w");
    
    if (pFile == nullptr) {
        throw std::runtime_error("File unable to be opened.");
    }
}

void Driver::CloseDevice()
{
    if (pFile) {
        fclose(pFile);
    }
}

void Driver::InitializeBulbs()
{
    // NOTE: This actually performs a very special
    // function on first initialization. The bulbs
    // need to be sequentially addressed in order to
    // initialize properly and pass-through their
    // runtime addressing information.
    for (uint8_t idx = 0; idx < 100; idx++) {
        SetBulb(idx, 0, 0, 0, 0);
    }
}

void Driver::SetBulb(uint8_t bulbIdx, uint8_t brightness, uint8_t red, uint8_t green, uint8_t blue)
{
    if (!pFile) {
        throw std::runtime_error("Device file not open.");
    }

    uint8_t command[5] = {};
    
    command[0] = (bulbIdx % 50);
    command[0] |= (bulbIdx / 50) << 6;

    command[1] = brightness;
    command[2] = blue;
    command[3] = green;
    command[4] = red;

    size_t result = fwrite(command, sizeof(uint8_t), sizeof(command), pFile);
    
    if (result != sizeof(command)) {
        throw std::runtime_error("Device write error.");
    } 

    fflush(pFile);
}
