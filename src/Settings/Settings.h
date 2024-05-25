#ifndef SETTINGS_H
#define SETTINGS_H

#include <Memory/Memory.h>
#include <Stamp.h>

struct Portion
{
    Stamp startFrom;
    uint8_t portion;
    uint8_t interval;
};

class Settings
{
private:
    enum Addresses
    {
        CONTROL_ADDR = 0,
        FIRST_TIME_ADDR = 1,
        DURATION_ADDR = 6,
        ANSWER_ADDR = 7
    };
    Memory settings[3] = {Memory(FIRST_TIME_ADDR), Memory(DURATION_ADDR), Memory(ANSWER_ADDR)};

public:
    Stamp firstTime = Stamp(2024, 04, 24, 12, 15, 0);
    byte duration = 0;
    bool answer = false;

    void load();
    void save();
};

#endif
