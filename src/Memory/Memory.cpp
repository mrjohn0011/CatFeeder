#include "Memory.h"

Memory::Memory(int addr) : address(addr) {}

void Memory::writeLong(const uint32_t &data)
{
    byte four = (data & 0xFF);
    byte three = ((data >> 8) & 0xFF);
    byte two = ((data >> 16) & 0xFF);
    byte one = ((data >> 24) & 0xFF);

    EEPROM.put(address, four);
    EEPROM.put(address + 1, three);
    EEPROM.put(address + 2, two);
    EEPROM.put(address + 3, one);
}

uint32_t Memory::readLong()
{
    long four = EEPROM.read(address);
    long three = EEPROM.read(address + 1);
    long two = EEPROM.read(address + 2);
    long one = EEPROM.read(address + 3);

    return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

void Memory::writeByte(const byte &data)
{
    EEPROM.put(address, data);
}

byte Memory::readByte()
{
    return EEPROM.read(address);
}
