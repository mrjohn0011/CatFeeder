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
        for (int i = 0; i < schedulesCount; ++i)
        {
            EEPROM.get(address, portions[i]);
            address = eepromAddress(i + 1);
            Serial.print("Loaded portion ");
            Serial.print(i);
            Serial.print(": ");
            Serial.print(portions[i].toString());
            Serial.print(" Last feed: ");
            Serial.println(portions[i].getLastFeed().toString());
        }
        address += sizeof(byte);
        speed = EEPROM.read(address);
        Serial.print("Loaded speed: ");
        Serial.println(speed);

        address += sizeof(byte);
        portionSize = EEPROM.read(address);
        Serial.print("Loaded portion size: ");
        Serial.println(portionSize);
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
    portionSize = 5;
    for (int i = 0; i < schedulesCount; ++i)
    {
        portions[i] = Portion(Stamp(2024, 9, 1, 12, 30, 0), 0, 1);
    }
    save();
}

void Settings::save()
{
    Serial.println("Writing version");
    EEPROM.write(0, actualVersion);
    int address = eepromAddress(0);
    Serial.println("Saving settings");
    Serial.print("Writing address ");
    Serial.println(address);

    for (int i = 0; i < schedulesCount; ++i)
    {
        EEPROM.put(address, portions[i]);
        address += portionObjectSize;
    }

    address += 1;
    EEPROM.put(address, speed);

    address += 1;
    EEPROM.put(address, portionSize);
}

Datime Settings::getNextFeed(Stamp currentTime)
{
    Datime nextFeed = Datime(0, 0, 0, 0, 0, 0);
    for (int i = 0; i < schedulesCount; ++i)
    {
        Datime currentPortion = portions[i].getNextFeed(currentTime);
        if (currentPortion.getUnix() < nextFeed.getUnix())
        {
            nextFeed = currentPortion;
        }
    }
    return nextFeed;
}