#include <MenuSelector/MenuSelector.h>
#include <Settings/Settings.h>
#include <Feeder/Feeder.h>
#include <MicroDS3231.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
MenuSelector selector(&lcd, 0, 4);
Feeder feeder(200, 8, 9, 10, 11);
Settings config;
MicroDS3231 rtc;

void setupCurrentTime()
{
  Stamp nowTime = selector.selectDateTime(config.firstTime);
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
    {"Feed now", setupFeedNow},
    {"Set clock", setupCurrentTime}};

void showError(String error)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(error);
  Serial.println(error);
  delay(1000);
}

void selfCheck()
{
  Serial.println("Start self check...");
  if (!rtc.begin())
  {
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
  }
}

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
