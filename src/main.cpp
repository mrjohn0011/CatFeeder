#include <MenuSelector/MenuSelector.h>
#include <Settings/Settings.h>
#include <Feeder/Feeder.h>
#include <MicroDS3231.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
MenuSelector selector(&lcd, 0, 3);
Feeder feeder(200, 3, 11, 12, 13, 2);
Settings config;
MicroDS3231 rtc;

void setupCurrentTime()
{
  Stamp nowTime = selector.selectDateTime(Stamp(rtc.getYear(), rtc.getMonth(), rtc.getDate(), rtc.getHours(), rtc.getMinutes(), rtc.getSeconds()));
  Datime dt = nowTime.get();
  Serial.print("Setting new current time: ");
  Serial.println(nowTime.toString());
  rtc.setTime(dt.second, dt.minute, dt.hour, dt.day, dt.month, dt.year);
  Serial.print("Clock updated to: ");
  Serial.print(rtc.getDateString());
  Serial.print(" ");
  Serial.println(rtc.getTimeString());
}

void setupFeedNow()
{
  int portionSize = selector.selectNumber(1, 0, 40);
  Serial.print("Manual feed selected: ");
  Serial.println(portionSize);
  feeder.feed(portionSize);
}

void showError(String error)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(error);
  Serial.println(error);
  delay(1000);
}

void setupSchedules()
{
  Menu scheduleMenu[] = {
      {"Schedule 1", config.portions[0].toString(), []()
       {
         config.portions[0] = selector.selectPortion(config.portions[0]);
         Serial.print("Schedule 1 set to: ");
         Serial.println(config.portions[0].toString());
       }},
      {"Schedule 2", config.portions[1].toString(), []()
       {
         config.portions[1] = selector.selectPortion(config.portions[1]);
         Serial.print("Schedule 2 set to: ");
         Serial.println(config.portions[1].toString());
       }},
      {"Schedule 3", config.portions[2].toString(), []()
       {
         config.portions[2] = selector.selectPortion(config.portions[2]);
         Serial.print("Schedule 3 set to: ");
         Serial.println(config.portions[2].toString());
       }},
      {"Schedule 4", config.portions[3].toString(), []()
       {
         config.portions[3] = selector.selectPortion(config.portions[3]);
         Serial.print("Schedule 4 set to: ");
         Serial.println(config.portions[3].toString());
       }}};

  MenuSelector scheduleSelector(&lcd, 0, 4);
  scheduleSelector.setMainMenu(scheduleMenu);
  scheduleSelector.showMainMenu();
}

Menu menus[] = {
    {"Feed now", "1", setupFeedNow},
    {"Set clock", "", setupCurrentTime},
    {"Set schedules", "", setupSchedules},
};

void selfCheck()
{
  Serial.println("Start self check...");
  if (!rtc.begin())
  {
    menus[1].defaultValue = " Clock not found";
    showError("Clock not found");
  }
  else
  {
    Serial.print("Clock checked: ");
    Serial.print(rtc.getDateString());
    Serial.print(" ");
    Serial.println(rtc.getTimeString());
  }

  if (rtc.lostPower())
  {
    showError("No clock battery");
    rtc.setTime(BUILD_SEC, BUILD_MIN, BUILD_HOUR, BUILD_DAY, BUILD_MONTH, BUILD_YEAR);
    menus[1].defaultValue = " No battery";
  }
}

void setup()
{
  lcd.begin(16, 2);
  Serial.begin(9600);
  pinMode(A0, INPUT_PULLUP);
  selfCheck();
  config.load();
  selector.setMainMenu(menus);
  selector.showMainMenu();
}

void loop()
{
  selector.setMainMenu(menus);
  selector.showMainMenu();
}
