#include <MenuSelector/MenuSelector.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
MenuSelector selector(&lcd, 0, 2);

void setupFirstDate()
{
  Stamp date = selector.selectDateTime(Stamp(2024, 04, 23, 12, 15, 0));
  Serial.print("Date selected: ");
  Serial.println(date.toString());
}

void setupDuration()
{
  int number = selector.selectNumber(15, 0, 50);
  Serial.print("Number selected: ");
  Serial.println(number);
}

Menu menus[] = {
    {"First date", setupFirstDate},
    {"Duration", setupDuration}};

void setup()
{
  lcd.begin(16, 2);
  Serial.begin(9600);
  pinMode(A0, INPUT_PULLUP);
  selector.setMainMenu(menus);
  selector.showMainMenu();
}

void loop()
{
}
