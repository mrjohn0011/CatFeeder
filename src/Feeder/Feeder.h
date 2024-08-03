#ifndef FEEDER_H
#define FEEDER_H

#include <Arduino.h>
#include <Stepper.h>

class Feeder
{
public:
    Feeder(int totalSteps, int pin1, int pin2, int pin3, int pin4, uint8_t mosfetPin);
    void feed(int portions);

private:
    Stepper motor;
    uint8_t mosfetPin = 2;
    int speed = 30;
    int totalSteps = 200;
    int onePortionAngle = 90;
};

#endif
