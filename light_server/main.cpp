#include <iostream>
#include <functional>
#include "LightsCore.h"

using namespace std;

void PrintDebugString(const string & debugText)
{
    cout << debugText;
}

int main (int argc, char** argv)
{
    LightsCore lc;
    lc.AttachDebugPrinter(&PrintDebugString);
    lc.Run();

    return 0;
}
