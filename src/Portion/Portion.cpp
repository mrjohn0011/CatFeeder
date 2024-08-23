#include "Portion.h"

Portion::Portion(Stamp startFrom, uint8_t amount, uint8_t interval) : startFrom(startFrom), amount(amount), interval(interval)
{
    this->setLastFeed(Datime(0, 0, 0, 0, 0, 0));
}

String Portion::toString()
{
    char buffer[17]; // 16 chars and 1 finishing zero
    Datime dt = startFrom.get();
    snprintf(buffer, sizeof(buffer), "%02d.%02d %02d:%02d %u %u",
             dt.day, dt.month, dt.hour, dt.minute, amount, interval);

    return String(buffer);
}

Datime Portion::getLastFeed()
{
    return lastFeed.get();
}

void Portion::setLastFeed(Datime lastFeed)
{
    this->lastFeed.set(lastFeed);
}

bool Portion::isTimeToFeed(Stamp currentTime)
{
    if (this->interval == 0)
    {
        Serial.println("Interval is 0");
        return false;
    }

    Datime nowTime = currentTime.get();
    Datime start = startFrom.get();
    Datime last = lastFeed.get();

    int daysFromLastFeeding = (nowTime.getUnix() - last.getUnix()) / 86400;

    if (daysFromLastFeeding == 0)
    {
        Serial.println("0 days from last feeding");
        return false;
    }

    int daysElapsed = (nowTime.getUnix() - start.getUnix()) / 86400;

    if (daysElapsed % interval != 0)
    {
        Serial.print("Days elapsed: ");
        Serial.print(daysElapsed);
        Serial.println(" - Too few days elapsed");
        return false;
    }

    if (nowTime.hour == start.hour && nowTime.minute == start.minute)
    {
        return true;
    }

    Serial.println("Not time to feed");
    return false;
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