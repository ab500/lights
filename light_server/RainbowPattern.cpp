#include "RainbowPattern.h"

#include <cstdlib>

void RainbowPattern::Tick(LightBoard & lightBoard)
{
    for (int i = 0; i < LightBoard::BulbCount; i++) {
        BulbCommand cmd = {};

        cmd.brightness = 200;
        cmd.red = rand() % 15;
        cmd.green = rand() % 15;
        cmd.blue = rand() % 15;
        lightBoard.SetBulb(i, cmd);
    }
}
