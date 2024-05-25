#include "Feeder.h"

Feeder::Feeder(int totalSteps, int pin1, int pin2, int pin3, int pin4) : totalSteps(totalSteps), motor(totalSteps, pin1, pin2, pin3, pin4)
{
    motor.setSpeed(this->speed);
}

void Feeder::feed(int portions)
{
    if (portions > 0)
    {
        motor.step(this->totalSteps * this->onePortionAngle * portions / 360);
    }
}
