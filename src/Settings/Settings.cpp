#include "Settings.h"

void Settings::load()
{
    byte version = EEPROM.read(0);
    if (version == actualVersion)
    {
        // When it is not the first run after the firmware update
        int address = eepromAddress(0);
        Serial.print("Reading address ");
        Serial.println(address);
        for (int i = 0; i < portionCount; ++i)
        {
            EEPROM.get(address, portions[i]);
            address += portionSize;
            Serial.print("Loaded portion ");
            Serial.print(i);
            Serial.print(": ");
            Serial.print(portions[i].toString());
            Serial.print(" Last feed: ");
            Serial.println(portions[i].getLastFeed().toString());
        }
        address += 1;
        speed = EEPROM.read(address);
        Serial.print("Loaded speed: ");
        Serial.println(speed);
    }
    else
    {
        // When version is not correct
        Serial.print("Reseting because config is outdated. Version is: ");
        Serial.println(version);
        Serial.print("Expected version is: ");
        Serial.println(actualVersion);
        reset();
    }
}

void Settings::reset()
{
    speed = 5;
    for (int i = 0; i < portionCount; ++i)
    {
        portions[i] = Portion(Stamp(2024, 9, 1, 12, 30, 0), 0, 1);
    }
    save();
}

void Settings::save()
{
    Serial.println("Writing version");
    EEPROM.put(0, actualVersion);
    int address = eepromAddress(0);
    Serial.println("Saving settings");
    Serial.print("Writing address ");
    Serial.println(address);

    for (int i = 0; i < portionCount; ++i)
    {
        EEPROM.put(address, portions[i]);
        address += portionSize;
    }

    address += 1;
    EEPROM.put(address, speed);
}
