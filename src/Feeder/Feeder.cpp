#include "Feeder.h"

Feeder::Feeder(uint8_t stepPin, uint8_t dirPin, uint8_t enable_pin) : stepPin(stepPin), dirPin(dirPin), enable_pin(enable_pin)
{
    pinMode(stepPin, OUTPUT);
    pinMode(dirPin, OUTPUT);
    pinMode(enable_pin, OUTPUT);
    digitalWrite(enable_pin, HIGH);
}

void Feeder::setSpeed(uint8_t s)
{
    this->speed = s;
    Serial.print("Speed set to: ");
    Serial.println(this->speed);
}

void Feeder::setPortionSize(uint8_t ps)
{
    this->portionSize = ps;
    Serial.print("Portion size set to: ");
    Serial.println(this->portionSize);
}

void Feeder::setDirection(bool clockwise)
{
    this->clockwise = clockwise;
}

void Feeder::feedPortion()
{
    digitalWrite(dirPin, clockwise ? HIGH : LOW);
    int speedDelay = 400 * (11 - speed);
    int totalSteps = 50 * portionSize;
    Serial.print("Speed delay:");
    Serial.println(speedDelay);
    Serial.print("Total steps:");
    Serial.println(totalSteps);
    for (int i = 0; i < totalSteps; i++)
    {
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(speedDelay);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(speedDelay);
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
