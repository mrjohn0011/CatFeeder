#include "MenuSelector.h"

MenuSelector::MenuSelector(LiquidCrystal *lcd, uint8_t selectedIndex, uint8_t maxIndex) : lcd(lcd), selectedIndex(selectedIndex), maxIndex(maxIndex)
{
    buttons.attach(RIGHT_BUTTON, 15);
    buttons.attach(UP_BUTTON, 155);
    buttons.attach(DOWN_BUTTON, 335);
    buttons.attach(LEFT_BUTTON, 507);
    buttons.attach(SELECT_BUTTON, 741);
    buttons.setWindow(70);
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
    Serial.print("Next index: ");
    if (selectedIndex < maxIndex - 1)
    {
        selectedIndex++;
        Serial.println(selectedIndex);
        showCurrentName();
    }
}

void MenuSelector::prev()
{
    Serial.print("Prev index: ");
    if (selectedIndex > 0)
    {
        selectedIndex--;
        Serial.println(selectedIndex);
        showCurrentName();
    }
}

void MenuSelector::showCurrentName()
{
    lcd->clear();
    Serial.print("Name: ");
    Serial.print(mainMenu[selectedIndex].name);
    showName(mainMenu[selectedIndex].name);
}

void MenuSelector::clearSecondRow()
{
    lcd->setCursor(0, 1);
    lcd->print("                ");
    lcd->setCursor(0, 1);
}

void MenuSelector::showMainMenu()
{
    showCurrentName();

    while (true)
    {
        leftButton.tick(buttons.status(LEFT_BUTTON));
        rightButton.tick(buttons.status(RIGHT_BUTTON));
        selectButton.tick(buttons.status(SELECT_BUTTON));

        if (leftButton.click())
            prev();

        if (rightButton.click())
            next();

        if (selectButton.click())
            mainMenu[selectedIndex].handler();
    }
}

int MenuSelector::selectNumber(int defaultNumber, int min, int max)
{
    lcd->setCursor(0, 1);
    lcd->print(defaultNumber);

    while (true)
    {
        upButton.tick(buttons.status(UP_BUTTON));
        downButton.tick(buttons.status(DOWN_BUTTON));
        selectButton.tick(buttons.status(SELECT_BUTTON));

        if ((upButton.click() || upButton.step()) && defaultNumber < max)
        {
            defaultNumber++;
            clearSecondRow();
            lcd->print(defaultNumber);
        }

        if ((downButton.click() || downButton.step()) && defaultNumber > min)
        {
            defaultNumber--;
            clearSecondRow();
            lcd->print(defaultNumber);
        }

        if (selectButton.click())
        {
            clearSecondRow();
            return defaultNumber;
        }
    }

    return 0;
}

Stamp MenuSelector::selectDateTime(Stamp defaultDateTime)
{
    struct DateComponent
    {
        uint16_t value;
        uint8_t position;
        uint8_t length;
        uint16_t min;
        uint16_t max;
    };

    Datime d = defaultDateTime.get();
    uint8_t index = 0;
    DateComponent dateParts[5] = {
        {d.day, 0, 2, 1, 31},
        {d.month, 3, 2, 1, 12},
        {d.year, 6, 4, 2024, 2105},
        {d.hour, 11, 2, 0, 23},
        {d.minute, 14, 2, 0, 59}};
    TimerMs blinkTimer = TimerMs(300, true, false);

    lcd->setCursor(0, 1);
    lcd->print(d.toString());
    bool showActivePart = true;

    while (true)
    {
        upButton.tick(buttons.status(UP_BUTTON));
        downButton.tick(buttons.status(DOWN_BUTTON));
        leftButton.tick(buttons.status(LEFT_BUTTON));
        rightButton.tick(buttons.status(RIGHT_BUTTON));
        selectButton.tick(buttons.status(SELECT_BUTTON));

        if (rightButton.click() && index < 5)
        {
            lcd->setCursor(0, 1);
            lcd->print(d.toString());
            showActivePart = true;
            index++;
        }

        if (leftButton.click() && index > 0)
        {
            lcd->setCursor(0, 1);
            lcd->print(d.toString());
            showActivePart = true;
            index--;
        }

        if (upButton.click() || upButton.step())
        {
            if ((index == 0 && dateParts[index].value < StampUtils::daysInMonth(dateParts[1].value, dateParts[2].value)) ||
                (index != 0 && dateParts[index].value < dateParts[index].max))
            {
                dateParts[index].value++;
                d.set(dateParts[2].value, dateParts[1].value, dateParts[0].value, dateParts[3].value, dateParts[4].value, 0);
            }
        }

        if (downButton.click() || downButton.step())
        {
            if (dateParts[index].value > dateParts[index].min)
            {
                dateParts[index].value--;
                d.set(dateParts[2].value, dateParts[1].value, dateParts[0].value, dateParts[3].value, dateParts[4].value, 0);
            }
        }

        if (selectButton.click())
        {
            blinkTimer.stop();
            clearSecondRow();
            defaultDateTime.set(d);
            return defaultDateTime;
        }

        if (blinkTimer.tick())
        {
            if (!upButton.holding() && !downButton.holding())
            {
                showActivePart = !showActivePart;
            }
            else
            {
                showActivePart = true;
            }

            lcd->setCursor(dateParts[index].position, 1);
            if (showActivePart)
            {
                if (dateParts[index].value < 10)
                    lcd->print("0");
                lcd->print(dateParts[index].value);
            }
            else
            {
                for (uint8_t i = 0; i < dateParts[index].length; i++)
                    lcd->print(" ");
            }
        }
    }
}