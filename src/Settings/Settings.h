#ifndef SETTINGS_H
#define SETTINGS_H

#include <EEPROM.h>
#include <Stamp.h>
#include <Portion/Portion.h>
class Settings
{
private:
    static const byte actualVersion = 1;
    static const uint32_t eepromStartAddress = 0;
    static const uint8_t schedulesCount = 4;
    static const uint32_t portionObjectSize = sizeof(Portion);
    uint32_t eepromAddress(int index)
    {
        return eepromStartAddress + sizeof(byte) + index * sizeof(Portion);
    }

public:
    uint8_t speed = 5;
    uint8_t portionSize = 5;
    Portion portions[4] = {
        Portion(Stamp(2024, 9, 1, 10, 30, 0), 1, 0),
        Portion(Stamp(2024, 9, 1, 12, 30, 0), 1, 0),
        Portion(Stamp(2024, 9, 1, 12, 30, 0), 1, 0),
        Portion(Stamp(2024, 9, 1, 12, 30, 0), 1, 0)};

    void load();
    void reset();
    void save();
    Datime getNextFeed(Stamp currentTime);
};

#endif
