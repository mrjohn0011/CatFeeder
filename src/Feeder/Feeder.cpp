#include "Feeder.h"

Feeder::Feeder(int stepsPerPortion, uint8_t stepPin, uint8_t dirPin) : stepsPerPortion(stepsPerPortion), stepPin(stepPin), dirPin(dirPin)
{
    pinMode(stepPin, OUTPUT);
    pinMode(dirPin, OUTPUT);
}

void Feeder::setSpeed(int speed)
{
    this->speed = speed;
}

void Feeder::setDirection(bool clockwise)
{
    this->clockwise = clockwise;
}

void Feeder::feedPortion()
{
    digitalWrite(dirPin, clockwise ? HIGH : LOW);
    for (int i = 0; i < stepsPerPortion; i++)
    {
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(speed);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(speed);
    }
    delay(1000);
}

void Feeder::feed(int portions)
{
    if (portions > 0)
    {
        for (int i = 0; i < portions; i++)
        {
            feedPortion();
        }
    }
}
