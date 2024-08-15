#ifndef FEEDER_H
#define FEEDER_H

#include <Arduino.h>

class Feeder
{
public:
    Feeder(int stepsPerPortion, uint8_t stepPin, uint8_t dirPin);
    void feed(int portions);
    void setSpeed(int speed);
    void setDirection(bool clockwise);

private:
    uint8_t stepPin = 3;
    uint8_t dirPin = 2;
    int speed = 800;
    bool clockwise = true;
    int stepsPerPortion = 50;

    void feedPortion();
};

#endif
