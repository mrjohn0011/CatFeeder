#ifndef MEMORY_H
#define MEMORY_H

#include <Arduino.h>
#include <EEPROM.h>

class Memory
{
public:
    Memory(int addr);

    void writeLong(const uint32_t &data);
    void writeByte(const byte &data);
    uint32_t readLong();
    byte readByte();

private:
    int address;
};

#endif
