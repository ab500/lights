#pragma once

#include "IPattern.h"

class RainbowPattern:
    public IPattern
{
public:
    virtual void Tick(LightBoard & lightBoard);        
};
