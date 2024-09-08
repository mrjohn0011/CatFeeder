#include "Settings.h"
#define LOGGING false

void Settings::load()
{
    byte version = EEPROM.read(eepromStartAddress);
#if LOGGING
    Serial.println("Loading version: " + String(version));
#endif
    if (version == actualVersion)
    {
        uint32_t address = eepromAddress(0);
#if LOGGING
        Serial.print("Reading address ");
        Serial.println(address);
#endif

        for (int i = 0; i < schedulesCount; i++)
        {
            EEPROM.get(address, portions[i]);
            address = eepromAddress(i + 1);
            Serial.print("Loaded portion " + String(i) + ": ");
            Serial.print(portions[i].toString());
            Serial.print("; Last feed: ");
            Serial.println(portions[i].getLastFeed().toString());
        }

        EEPROM.get(address + 1, speed);
        Serial.println("Loaded speed: " + String(speed));

        EEPROM.get(address + 2, portionSize);
        Serial.println("Loaded portion size: " + String(portionSize));
    }
}

void Settings::reset()
{
    speed = 5;
    portionSize = 5;
    for (int i = 0; i < schedulesCount; ++i)
    {
        portions[i] = Portion(Stamp(2024, 9, 1, 12, 30, 0), 1, 0);
    }
}

void Settings::save()
{
    EEPROM.write(eepromStartAddress, actualVersion);
    int address = eepromAddress(0);
    Serial.println("Saving settings");

    for (int i = 0; i < schedulesCount; i++)
    {
        EEPROM.put(address, portions[i]);
        address = eepromAddress(i + 1);
#if LOGGING
        Serial.print("Saved portion " + String(i) + ": ");
        Serial.print(portions[i].toString());
        Serial.print("; Last feed: ");
        Serial.println(portions[i].getLastFeed().toString());
#endif
    }

    EEPROM.put(address + 1, speed);
    EEPROM.put(address + 2, portionSize);
#if LOGGING
    Serial.println("Saved speed: " + String(speed));
    Serial.println("Saved portion size: " + String(portionSize));
#endif
}

Datime Settings::getNextFeed(Stamp currentTime)
{
    Datime nextFeed = Datime(0, 0, 0, 0, 0, 0);
    for (int i = 0; i < schedulesCount; ++i)
    {
        Datime currentPortion = portions[i].getNextFeed(currentTime);
        Serial.print("Next feed for " + String(i) + ": ");
        Serial.println(currentPortion.toString());
        if (currentPortion.getUnix() < nextFeed.getUnix())
        {
            nextFeed = currentPortion;
        }
    }
    return nextFeed;
}