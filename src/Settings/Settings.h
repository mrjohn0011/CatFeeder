#ifndef SETTINGS_H
#define SETTINGS_H

#include <EEPROM.h>
#include <Stamp.h>
#include <Portion/Portion.h>
class Settings
{
private:
    int eepromAddress(int index)
    {
        return sizeof(byte) + index * sizeof(Portion);
    }

public:
    Portion portions[4] = {
        Portion(Stamp(2024, 9, 1, 10, 30, 0), 0, 1),
        Portion(Stamp(2024, 9, 1, 12, 30, 0), 0, 1),
        Portion(Stamp(2024, 9, 1, 12, 30, 0), 0, 1),
        Portion(Stamp(2024, 9, 1, 12, 30, 0), 0, 1)};

    void load();
    void save();
};

#endif
