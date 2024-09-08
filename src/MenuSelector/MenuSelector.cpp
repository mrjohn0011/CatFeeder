#include "MenuSelector.h"
#define LOGGING false

MenuSelector::MenuSelector(LiquidCrystal *lcd, uint8_t selectedIndex, uint8_t maxIndex) : lcd(lcd), selectedIndex(selectedIndex), maxIndex(maxIndex)
{
    buttons.attach(RIGHT_BUTTON, 15);
    buttons.attach(UP_BUTTON, 155);
    buttons.attach(DOWN_BUTTON, 335);
    buttons.attach(LEFT_BUTTON, 507);
    buttons.attach(SELECT_BUTTON, 741);
    buttons.setWindow(70);
    inactiveTimer = TimerMs(10000, false, false);
}

void MenuSelector::setMainMenu(Menu *menus)
{
    mainMenu = menus;
}

void MenuSelector::showName(String name)
{
    lcd->setCursor(0, 0);
    if (selectedIndex > 0)
        lcd->print("< ");
    lcd->print(name);
    if (selectedIndex < maxIndex - 1)
        lcd->print(" >");
}

void MenuSelector::next()
{
    if (selectedIndex < maxIndex - 1)
    {
        selectedIndex++;
        showCurrentName();
    }
}

void MenuSelector::prev()
{
    if (selectedIndex > 0)
    {
        selectedIndex--;
        showCurrentName();
    }
}

void MenuSelector::showCurrentName()
{
#if LOGGING
    Serial.print("Show current name: ");
    Serial.println(mainMenu[selectedIndex].name);
#endif
    lcd->clear();
    showName(mainMenu[selectedIndex].name);
    lcd->setCursor(0, 1);
    lcd->print(mainMenu[selectedIndex].defaultValue);
}

void MenuSelector::clearSecondRow()
{
    lcd->setCursor(0, 1);
    lcd->print("                ");
    lcd->setCursor(0, 1);
}

bool MenuSelector::showMainMenuOnSelect()
{
    selectButton.tick(buttons.status(SELECT_BUTTON));

    if (selectButton.click())
    {
        showMainMenu();
        return true;
    }

    return false;
}

void MenuSelector::restartInactiveTimer(String message)
{
    // Serial.print("Clear inactive timer: ");
    // Serial.println(message);
    inactiveTimer.restart();
}

void MenuSelector::showMainMenu()
{
    this->selectedIndex = 0;
    showCurrentName();
    inactiveTimer.start();

    while (true)
    {
        leftButton.tick(buttons.status(LEFT_BUTTON));
        rightButton.tick(buttons.status(RIGHT_BUTTON));
        selectButton.tick(buttons.status(SELECT_BUTTON));

        if (buttons.pressed() != NO_BUTTON)
        {
            restartInactiveTimer("main menu");
        }

        if (leftButton.click())
            prev();

        if (rightButton.click())
            next();

        if (selectButton.click())
            mainMenu[selectedIndex].handler();

        if (inactiveTimer.tick())
        {
#if LOGGING
            Serial.println("Inactive timer in main menu");
#endif
            inactiveTimer.stop();
            return;
        }
    }
}

int MenuSelector::selectNumber(int defaultNumber, int min, int max)
{
    DateComponent cmp = {defaultNumber, 0, 16, min, max};
    cmp = editDateComponent(cmp);
    clearSecondRow();
    return cmp.value;
}

void MenuSelector::toggleDateComponent(DateComponent cmp, bool state)
{
    lcd->setCursor(cmp.position, 1);
    if (state)
    {
        if (cmp.max >= 10 && cmp.value < 10)
            lcd->print("0");
        lcd->print(cmp.value);
    }
    else
    {
        for (uint8_t i = 0; i < cmp.length; i++)
            lcd->print(" ");
    }
}

MenuSelector::DateComponent MenuSelector::editDateComponent(DateComponent cmp)
{
    TimerMs blinkTimer = TimerMs(400, true, false);
    bool componentBlinkState = true;

    while (true)
    {
        upButton.tick(buttons.status(UP_BUTTON));
        downButton.tick(buttons.status(DOWN_BUTTON));
        leftButton.tick(buttons.status(LEFT_BUTTON));
        rightButton.tick(buttons.status(RIGHT_BUTTON));
        selectButton.tick(buttons.status(SELECT_BUTTON));

        if (buttons.pressed() != NO_BUTTON)
        {
            restartInactiveTimer("edit date component");
        }

        if (rightButton.click() || leftButton.click() || selectButton.click())
        {
            blinkTimer.stop();
            toggleDateComponent(cmp, true);
            return cmp;
        }

        if ((upButton.click() || upButton.step()) && cmp.value < cmp.max)
        {
            cmp.value++;
        }

        if ((downButton.click() || downButton.step()) && cmp.value > cmp.min)
        {
            cmp.value--;
        }

        if (blinkTimer.tick())
        {
            if (!upButton.holding() && !downButton.holding())
            {
                componentBlinkState = !componentBlinkState;
            }
            else
            {
                componentBlinkState = true;
            }

            toggleDateComponent(cmp, componentBlinkState);
        }

        if (inactiveTimer.tick())
        {
#if LOGGING
            Serial.println("Inactive timer in edit date component");
#endif
            return cmp;
        }
    }
}

Stamp MenuSelector::selectDateTime(Stamp defaultDateTime)
{
    Datime d = defaultDateTime.get();
    uint8_t index = 0;
    DateComponent dateParts[5] = {
        {d.day, 0, 2, 1, StampUtils::daysInMonth(d.month, d.year)},
        {d.month, 3, 2, 1, 12},
        {d.year, 6, 4, 2024, 2105},
        {d.hour, 11, 2, 0, 23},
        {d.minute, 14, 2, 0, 59}};

    lcd->setCursor(0, 1);
    lcd->print(d.toString());

    while (true)
    {
        leftButton.tick(buttons.status(LEFT_BUTTON));
        rightButton.tick(buttons.status(RIGHT_BUTTON));
        selectButton.tick(buttons.status(SELECT_BUTTON));

        if (buttons.pressed() == NO_BUTTON)
        {
            dateParts[index] = editDateComponent(dateParts[index]);
            if (index == 1 || index == 2)
            {
                dateParts[0].max = StampUtils::daysInMonth(dateParts[1].value, dateParts[2].value);
                if (dateParts[0].value > dateParts[0].max)
                {
                    dateParts[0].value = dateParts[0].max;
                    toggleDateComponent(dateParts[0], true);
                }
            }
        }
        else
        {
            restartInactiveTimer("select date time");
        }

        if (rightButton.click() && index < 5)
        {
            index++;
        }

        if (leftButton.click() && index > 0)
        {
            index--;
        }

        if (selectButton.click())
        {
            clearSecondRow();
            d.set(dateParts[2].value, dateParts[1].value, dateParts[0].value, dateParts[3].value, dateParts[4].value, 0);
            defaultDateTime.set(d);
            return defaultDateTime;
        }

        if (inactiveTimer.tick())
        {
#if LOGGING
            Serial.println("Inactive timer in select date time");
#endif
            return defaultDateTime;
        }
    }
}

Stamp MenuSelector::selectTime(Stamp defaultTime)
{
    Datime d = defaultTime.get();
    uint8_t index = 0;
    DateComponent dateParts[3] = {
        {d.hour, 0, 2, 0, 23},
        {d.minute, 3, 2, 0, 59},
        {d.second, 6, 2, 0, 59}};

    lcd->setCursor(0, 1);
    lcd->print(d.timeToString());

    while (true)
    {
        leftButton.tick(buttons.status(LEFT_BUTTON));
        rightButton.tick(buttons.status(RIGHT_BUTTON));
        selectButton.tick(buttons.status(SELECT_BUTTON));

        if (buttons.pressed() == NO_BUTTON)
        {
            dateParts[index] = editDateComponent(dateParts[index]);
        }
        else
        {
            restartInactiveTimer("select time");
        }

        if (rightButton.click() && index < 2)
        {
            index++;
        }

        if (leftButton.click() && index > 0)
        {
            index--;
        }

        if (selectButton.click())
        {
            clearSecondRow();
            d.set(dateParts[0].value, dateParts[1].value, dateParts[2].value);
            defaultTime.set(d);
            return defaultTime;
        }

        if (inactiveTimer.tick())
        {
#if LOGGING
            Serial.println("Inactive timer in select time");
#endif
            return defaultTime;
        }
    }
}

bool MenuSelector::selectBoolean(bool defaultValue)
{
    lcd->setCursor(0, 1);
    lcd->print(defaultValue ? "YES" : "NO");

    while (true)
    {
        upButton.tick(buttons.status(UP_BUTTON));
        downButton.tick(buttons.status(DOWN_BUTTON));
        selectButton.tick(buttons.status(SELECT_BUTTON));

        if (buttons.pressed() != NO_BUTTON)
        {
            restartInactiveTimer("select boolean");
        }

        if (upButton.click() || downButton.click())
        {
            defaultValue = !defaultValue;
            clearSecondRow();
            lcd->print(defaultValue ? "YES" : "NO");
        }

        if (selectButton.click())
        {
            clearSecondRow();
            return defaultValue;
        }

        if (inactiveTimer.tick())
        {
#if LOGGING
            Serial.println("Inactive timer in select boolean");
#endif
            return defaultValue;
        }
    }
}

Portion MenuSelector::selectPortion(Portion defaultPortion)
{
    // 05.12 12:30 3 10
    Datime d = defaultPortion.getStartFrom();
    uint8_t index = 0;
    DateComponent dateParts[6] = {
        {d.day, 0, 2, 1, StampUtils::daysInMonth(d.month, d.year)},
        {d.month, 3, 2, 1, 12},
        {d.hour, 6, 2, 0, 23},
        {d.minute, 9, 2, 0, 59},
        {defaultPortion.getAmount(), 12, 1, 0, 9},
        {defaultPortion.getInterval(), 14, 2, 1, 99}};

    lcd->setCursor(0, 1);
    lcd->print(defaultPortion.toString());

    while (true)
    {
        leftButton.tick(buttons.status(LEFT_BUTTON));
        rightButton.tick(buttons.status(RIGHT_BUTTON));
        selectButton.tick(buttons.status(SELECT_BUTTON));

        if (buttons.pressed() == NO_BUTTON)
        {
            dateParts[index] = editDateComponent(dateParts[index]);
            if (index == 1 || index == 2)
            {
                dateParts[0].max = StampUtils::daysInMonth(dateParts[1].value, dateParts[2].value);
                if (dateParts[0].value > dateParts[0].max)
                {
                    dateParts[0].value = dateParts[0].max;
                    toggleDateComponent(dateParts[0], true);
                }
            }
        }
        else
        {
            restartInactiveTimer("select portion");
        }

        if (rightButton.click() && index < 5)
        {
            index++;
        }

        if (leftButton.click() && index > 0)
        {
            index--;
        }

        if (selectButton.click())
        {
            clearSecondRow();
            d.set(d.year, dateParts[1].value, dateParts[0].value, dateParts[2].value, dateParts[3].value, 0);
            defaultPortion.setStartFrom(d);
            defaultPortion.setAmount(dateParts[4].value);
            defaultPortion.setInterval(dateParts[5].value);
            return defaultPortion;
        }

        if (inactiveTimer.tick())
        {
#if LOGGING
            Serial.println("Inactive timer in select portion");
#endif
            return defaultPortion;
        }
    }
};
