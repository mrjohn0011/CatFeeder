#include "Settings.h"

void Settings::load()
{
    if (Memory(CONTROL_ADDR).readByte() == 1)
    {
        firstTime = Stamp(settings[0].readLong());
        duration = settings[1].readByte();
        answer = settings[2].readByte() == 0 ? false : true;
    }
    else
    {
        save();
    }
}

void Settings::save()
{
    Serial.println("Saving settings");
    Memory(CONTROL_ADDR).writeByte(1);
    settings[0].writeLong(firstTime.getUnix());
    settings[1].writeLong(duration);
    settings[2].writeByte(answer ? 1 : 0);
}
