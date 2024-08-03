#include "Feeder.h"

Feeder::Feeder(int totalSteps, int pin1, int pin2, int pin3, int pin4, uint8_t mosfetPin) : totalSteps(totalSteps), motor(totalSteps, pin1, pin2, pin3, pin4, mosfetPin)
{
    motor.setSpeed(this->speed);
    pinMode(mosfetPin, OUTPUT);
}

void Feeder::feed(int portions)
{
    if (portions > 0)
    {
        digitalWrite(this->mosfetPin, HIGH);
        for (int i = 0; i < portions; i++)
        {
            motor.step(this->totalSteps * this->onePortionAngle / 360);
        }
        digitalWrite(this->mosfetPin, LOW);
    }
}
