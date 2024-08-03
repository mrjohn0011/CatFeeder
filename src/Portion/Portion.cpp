#include "Portion.h"

Portion::Portion(Stamp startFrom, uint8_t amount, uint8_t interval) : startFrom(startFrom), amount(amount), interval(interval)
{
}

String Portion::toString()
{
    char buffer[17]; // 16 chars and 1 finishing zero
    Datime dt = startFrom.get();
    snprintf(buffer, sizeof(buffer), "%02d.%02d %02d:%02d %u %u",
             dt.day, dt.month, dt.hour, dt.minute, amount, interval);

    return String(buffer);
}

void Portion::setAmount(uint8_t amount)
{
    this->amount = amount;
}

void Portion::setInterval(uint8_t interval)
{
    this->interval = interval;
}

void Portion::setStartFrom(Datime startFrom)
{
    this->startFrom.set(startFrom);
}

Datime Portion::getStartFrom()
{
    return startFrom.get();
}

uint8_t Portion::getAmount()
{
    return amount;
}

uint8_t Portion::getInterval()
{
    return interval;
}