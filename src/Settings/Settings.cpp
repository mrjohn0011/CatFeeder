#include "Settings.h"

void Settings::load()
{
    byte firstByte = EEPROM.read(0);
    if (firstByte == 1)
    {
        // When it is not the first run after the firmware update
        int address = eepromAddress(0);
        Serial.print("Reading address ");
        Serial.println(address);
        for (int i = 0; i < portionCount; ++i)
        {
            EEPROM.get(address, portions[i]);
            address += portionSize;
        }
    }
    else
    {
        // When it is the first run, we should write default settings
        Serial.print("First byte: ");
        Serial.println(firstByte);
        save();
    }
}

void Settings::reset()
{
    for (int i = 0; i < portionCount; ++i)
    {
        portions[i] = Portion(Stamp(2024, 9, 1, 12, 30, 0), 0, 1);
    }
    save();
}

void Settings::save()
{
    Serial.println("Writing first byte");
    EEPROM.write(0, 1);
    int address = eepromAddress(0);
    Serial.println("Saving settings");
    Serial.print("Writing address ");
    Serial.println(address);

    for (int i = 0; i < portionCount; ++i)
    {
        EEPROM.put(address, portions[i]);
        address += portionSize;
    }
}
