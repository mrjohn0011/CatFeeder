#ifndef SETTINGS_H
#define SETTINGS_H

#include <EEPROM.h>
#include <Stamp.h>
#include <Portion/Portion.h>
class Settings
{
private:
    static const int actualVersion = 2;
    static const int eepromStartAddress = 0;
    static const int portionCount = 4;
    static const int portionSize = sizeof(Portion);
    int eepromAddress(int index)
    {
        return sizeof(byte) + index * sizeof(Portion);
    }

public:
    uint8_t speed = 5;
    Portion portions[4] = {
        Portion(Stamp(2024, 9, 1, 10, 30, 0), 0, 1),
        Portion(Stamp(2024, 9, 1, 12, 30, 0), 0, 1),
        Portion(Stamp(2024, 9, 1, 12, 30, 0), 0, 1),
        Portion(Stamp(2024, 9, 1, 12, 30, 0), 0, 1)};

    void load();
    void reset();
    void save();
};

#endif
