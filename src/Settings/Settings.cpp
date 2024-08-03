#include "Settings.h"

void Settings::load()
{
    if (EEPROM.read(0) == 1)
    {
        // When it is not the first run after the firmware update
        Serial.print("Reading address ");
        Serial.println(eepromAddress(0));
        EEPROM.get(eepromAddress(0), portions);
    }
    else
    {
        // When it is the first run, we should write default settings
        save();
    }
}

void Settings::save()
{
    Serial.println("Saving settings");
    Serial.print("Writing address ");
    Serial.println(eepromAddress(0));
    EEPROM.put(eepromAddress(0), portions);
}
