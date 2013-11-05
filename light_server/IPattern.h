#pragma once

#include "LightBoard.h"

struct IPattern
{
    virtual void Tick(LightBoard & lightBoard) = 0;
};
