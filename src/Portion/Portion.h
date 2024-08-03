#ifndef PORTION_H
#define PORTION_H

#include <Stamp.h>

class Portion
{
private:
    Stamp startFrom;  // The date this schedule should be active from
    uint8_t amount;   // The amount of feeding in portions
    uint8_t interval; // The amount of days to repeat. For example, every "3" days

public:
    Portion(Stamp startFrom, uint8_t amount, uint8_t interval);
    void setAmount(uint8_t amount);
    void setInterval(uint8_t interval);
    void setStartFrom(Datime startFrom);
    Datime getStartFrom();
    uint8_t getAmount();
    uint8_t getInterval();
    String toString();
};
#endif