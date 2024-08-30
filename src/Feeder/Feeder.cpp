#include "Feeder.h"

Feeder::Feeder(int stepsPerPortion, uint8_t stepPin, uint8_t dirPin, uint8_t enable_pin) : stepsPerPortion(stepsPerPortion), stepPin(stepPin), dirPin(dirPin), enable_pin(enable_pin)
{
    pinMode(stepPin, OUTPUT);
    pinMode(dirPin, OUTPUT);
    pinMode(enable_pin, OUTPUT);
    digitalWrite(enable_pin, HIGH);
}

void Feeder::setSpeed(uint8_t speedLevel)
{
    this->speed = 400 * (11 - speedLevel);
    Serial.print("Speed delay set to: ");
    Serial.println(this->speed);
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
    delay(500);
}

void Feeder::feed(int portions)
{
    if (portions > 0)
    {
        digitalWrite(enable_pin, LOW);
        for (int i = 0; i < portions; i++)
        {
            feedPortion();
        }
        digitalWrite(enable_pin, HIGH);
    }
}
