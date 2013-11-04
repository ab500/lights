#include "LightBoard.h"

#include <stdexcept>

void LightBoard::SetBulb(int bulbIdx, BulbCommand cmd)
{
    if (bulbIdx > BulbCount - 1) {
        throw std::runtime_error("Bulb count exceeded maximum value.");
    }
   m_pendingUpdates.push_back(std::pair<int, BulbCommand>(bulbIdx, cmd)); 
}

void LightBoard::GetUpdate(std::vector<std::pair<int, BulbCommand>> & deltaList)
{
    // In the future we could do something more intelligent here, filtering
    // out bulbs that haven't changed, ordering them in a certain way, etc.
    for (auto iter = m_pendingUpdates.begin(); iter != m_pendingUpdates.end(); iter++) {
        deltaList.push_back(*iter);
    }
    m_pendingUpdates.clear();
}

void LightBoard::Reset(bool generateClearCommands)
{
    m_pendingUpdates.clear();
    if (generateClearCommands) {
        for (int i = 0; i < BulbCount; i++) {
            BulbCommand bc = {};
            bc.brightness = 0;
            bc.red = 0;
            bc.green = 0;
            bc.blue = 0;
            m_pendingUpdates.push_back(std::pair<int, BulbCommand>(i, bc));
        }
    }
}
