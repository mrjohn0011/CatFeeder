#include <MenuSelector/MenuSelector.h>
#include <Memory/Memory.h>
#include <Settings/Settings.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
MenuSelector selector(&lcd, 0, 4);
Settings config;

void setupFirstDate()
{
  config.firstTime = selector.selectDateTime(config.firstTime);
  config.save();
  Serial.print("Date selected: ");
  Serial.println(config.firstTime.toString());
}

void setupDuration()
{
  config.duration = selector.selectNumber(config.duration, 0, 127);
  config.save();
  Serial.print("Number selected: ");
  Serial.println(config.duration);
}

void setupYesNo()
{
  config.answer = selector.selectBoolean(config.answer);
  config.save();
  Serial.print("Selected: ");
  Serial.println(config.answer);
}

void setupTime()
{
  Stamp time = selector.selectTime(Stamp(12, 15, 0));
  Serial.print("Selected: ");
  Serial.println(time.timeToString());
}

Menu menus[] = {
    {"First date", setupFirstDate},
    {"Duration", setupDuration},
    {"Answer", setupYesNo},
    {"Time", setupTime},
};

void setup()
{
  lcd.begin(16, 2);
  Serial.begin(9600);
  pinMode(A0, INPUT_PULLUP);
  config.load();
  selector.setMainMenu(menus);
  selector.showMainMenu();
}

void loop()
{
}
