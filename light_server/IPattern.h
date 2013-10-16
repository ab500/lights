#pragma once

#include "LightBoard.h"

class IPattern
{
    virtual void Tick(LightBoard & lightBoard);
};
