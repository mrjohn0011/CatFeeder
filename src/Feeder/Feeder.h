#ifndef FEEDER_H
#define FEEDER_H

#include <Arduino.h>

class Feeder
{
public:
    Feeder(uint8_t stepPin, uint8_t dirPin, uint8_t enable_pin);
    void feed(int portions);
    void setSpeed(uint8_t speedLevel);
    void setPortionSize(uint8_t ps);
    void setDirection(bool clockwise);

private:
    uint8_t stepPin = 3;
    uint8_t dirPin = 2;
    uint8_t enable_pin = 12;
    int speed = 5;
    bool clockwise = true;
    int portionSize = 5;

    void feedPortion();
};

#endif
