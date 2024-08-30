#ifndef FEEDER_H
#define FEEDER_H

#include <Arduino.h>

class Feeder
{
public:
    Feeder(int stepsPerPortion, uint8_t stepPin, uint8_t dirPin, uint8_t enable_pin);
    void feed(int portions);
    void setSpeed(uint8_t speedLevel);
    void setDirection(bool clockwise);

private:
    uint8_t stepPin = 3;
    uint8_t dirPin = 2;
    uint8_t enable_pin = 12;
    int speed = 2400;
    bool clockwise = true;
    int stepsPerPortion = 50;

    void feedPortion();
};

#endif
