#ifndef MENUSELECTOR_H
#define MENUSELECTOR_H

#include <Arduino.h>
#include <LiquidCrystal.h>
#include <Wire.h>
#include <Stamp.h>
#include <TimerMs.h>
#include <AnalogKey.h>
#include <EncButton.h>

#define EB_DEB_TIME 20
#define EB_CLICK_TIME 50

typedef void (*MenuHandler)();
struct Menu
{
    String name;
    MenuHandler handler;
};

enum MenuButton
{
    NO_BUTTON = -1,
    RIGHT_BUTTON = 0,
    UP_BUTTON = 1,
    DOWN_BUTTON = 2,
    LEFT_BUTTON = 3,
    SELECT_BUTTON = 4
};

class MenuSelector
{
private:
    const int ButtonPin = A0;
    uint8_t selectedIndex = 0;
    uint8_t maxIndex = 0;
    Menu *mainMenu;
    LiquidCrystal *lcd;
    AnalogKey<A0, 5> buttons;
    VirtButton upButton;
    VirtButton downButton;
    VirtButton leftButton;
    VirtButton rightButton;
    VirtButton selectButton;
    void next();
    void prev();
    void clearSecondRow();
    void showName(String name);
    void showCurrentName();

public:
    MenuSelector(LiquidCrystal *lcd, uint8_t selectedIndex, uint8_t maxIndex);
    Stamp selectDateTime(Stamp defaultDateTime);
    void setMainMenu(Menu *menus);
    int selectNumber(int defaultNumber, int min, int max);
    void showMainMenu();
};
#endif