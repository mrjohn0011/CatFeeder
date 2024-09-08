#include "Feeder.h"
#define LOGGING false

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
#if LOGGING
    Serial.print("Speed set to: ");
    Serial.println(this->speed);
#endif
}

void Feeder::setPortionSize(uint8_t ps)
{
    this->portionSize = ps;
#if LOGGING
    Serial.print("Portion size set to: ");
    Serial.println(this->portionSize);
#endif
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
#if LOGGING
    Serial.print("Speed delay:");
    Serial.println(speedDelay);
    Serial.print("Total steps:");
    Serial.println(totalSteps);
#endif
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
