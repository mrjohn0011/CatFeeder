#include <MenuSelector/MenuSelector.h>
#include <Settings/Settings.h>
#include <Feeder/Feeder.h>
#include <MicroDS3231.h>

#define MENU_COUNT 4
#define SCHEDULE_COUNT 4
#define USE_REAL_RTC true
#define LOGGING true

#define FAKE_RTC_YEAR 2024
#define FAKE_RTC_MONTH 9
#define FAKE_RTC_DAY 8
#define FAKE_RTC_HOUR 16
#define FAKE_RTC_MIN 38
#define FAKE_RTC_SEC 15

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
MenuSelector selector(&lcd, 0, MENU_COUNT + SCHEDULE_COUNT);

Feeder feeder(3, 2, 12);
TimerMs feederTimer(20000, true, false);
Settings config;
#if USE_REAL_RTC
MicroDS3231 rtc;
#endif

void setupCurrentTime()
{
#if USE_REAL_RTC
  Stamp nowTime = selector.selectDateTime(Stamp(rtc.getYear(), rtc.getMonth(), rtc.getDate(), rtc.getHours(), rtc.getMinutes(), rtc.getSeconds()));
#else
  Stamp nowTime = selector.selectDateTime(Stamp(FAKE_RTC_YEAR, FAKE_RTC_MONTH, FAKE_RTC_DAY, FAKE_RTC_HOUR, FAKE_RTC_MIN, FAKE_RTC_SEC));
#endif
  Datime dt = nowTime.get();
#if LOGGING
  Serial.println("Current time: " + nowTime.toString());
  Serial.print("Setting new current time: ");
  Serial.println(nowTime.toString());
#endif
#if USE_REAL_RTC
  rtc.setTime(dt.second, dt.minute, dt.hour, dt.day, dt.month, dt.year);
#if LOGGING
  Serial.print("Clock updated to: ");
  Serial.print(rtc.getDateString());
  Serial.print(" ");
  Serial.println(rtc.getTimeString());
#endif
#endif
}

void setupFeedNow()
{
  int portionSize = selector.selectNumber(1, 0, 40);
#if LOGGING
  Serial.print("Manual feed selected: ");
  Serial.println(portionSize);
  feeder.feed(portionSize);
#endif
}

void showError(String error)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(error);
#if LOGGING
  Serial.println(error);
#endif
  delay(1000);
}

Menu menus[MENU_COUNT + SCHEDULE_COUNT];

void setupSpeed()
{
  config.speed = selector.selectNumber(config.speed, 1, 10);
  feeder.setSpeed(config.speed);
  config.save();
  menus[6].defaultValue = String(config.speed);
#if LOGGING
  Serial.print("Speed value set to: ");
  Serial.println(config.speed);
#endif
}

void setupPortionSize()
{
  config.portionSize = selector.selectNumber(config.portionSize, 1, 16);
  feeder.setPortionSize(config.portionSize);
  config.save();
  menus[7].defaultValue = String(config.portionSize);
#if LOGGING
  Serial.print("Portion size: ");
  Serial.println(config.portionSize);
#endif
}

void scheduleSetter(uint8_t i)
{
  config.portions[i] = selector.selectPortion(config.portions[i]);
#if LOGGING
  Serial.print("Schedule " + String(i + 1) + " set to: ");
  Serial.println(config.portions[i].toString());
#endif
  config.save();
  menus[i + 2].defaultValue = config.portions[i].toString();
}

void initMenus()
{
  menus[0] = {"Feed now", "1", setupFeedNow};
  menus[1] = {"Set clock", "", setupCurrentTime};
  menus[2] = {"Schedule 1", config.portions[0].toString(), []()
              { scheduleSetter(0); }};
  menus[3] = {"Schedule 2", config.portions[1].toString(), []()
              { scheduleSetter(1); }};
  menus[4] = {"Schedule 3", config.portions[2].toString(), []()
              { scheduleSetter(2); }};
  menus[5] = {"Schedule 4", config.portions[3].toString(), []()
              { scheduleSetter(3); }};
  menus[6] = {"Motor speed", String(config.speed), setupSpeed};
  menus[7] = {"Portion size", String(config.portionSize), setupPortionSize};
}

bool checkRTC()
{
#if USE_REAL_RTC
  Wire.beginTransmission(0x68);
  if (Wire.endTransmission() != 0)
  {
    showError("Clock not found");
    return false;
  }

  if (rtc.lostPower())
  {
    showError("No clock battery");
    return false;
  }
#endif
  return true;
}

void selfCheck()
{
#if USE_REAL_RTC
#if LOGGING
  Serial.println("Start self check...");
#endif
  if (!rtc.begin())
  {
    menus[1].defaultValue = " Clock not found";
    showError("Clock not found");
  }
  else
  {
#if LOGGING
    Serial.print("Clock checked: ");
    Serial.print(rtc.getDateString());
    Serial.print(" ");
    Serial.println(rtc.getTimeString());
#endif
  }

  if (rtc.lostPower())
  {
    showError("No clock battery");
    rtc.setTime(BUILD_SEC, BUILD_MIN, BUILD_HOUR, BUILD_DAY, BUILD_MONTH, BUILD_YEAR);
    menus[1].defaultValue = " No battery";
  }
#endif
#if LOGGING
  Serial.println("Self check done");
#endif
}

void regularScheduleCheck()
{
#if LOGGING
  Serial.println("Regular schedule checking...");
#endif
  if (checkRTC())
  {
#if USE_REAL_RTC
    Stamp nowTime = Stamp(rtc.getYear(), rtc.getMonth(), rtc.getDate(), rtc.getHours(), rtc.getMinutes(), 0);
#else
    Stamp nowTime = Stamp(FAKE_RTC_YEAR, FAKE_RTC_MONTH, FAKE_RTC_DAY, FAKE_RTC_HOUR, FAKE_RTC_MIN, FAKE_RTC_SEC);
#endif
    bool needSave = false;

    for (int i = 0; i < SCHEDULE_COUNT; i++)
    {
      if (config.portions[i].isTimeToFeed(nowTime))
      {
        feeder.feed(config.portions[i].getAmount());
        config.portions[i].setLastFeed(nowTime.get());
#if LOGGING
        Serial.print("Schedule " + String(i + 1) + " fed: ");
        Serial.println(config.portions[i].getAmount());
#endif
        needSave = true;
        break;
      }
    }

    if (needSave)
    {
      config.save();
    }
  }
  else
  {
    lcd.setCursor(0, 1);
    lcd.print("Press select");
  }
}

void showNextFeedTime()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  if (checkRTC())
  {
#if USE_REAL_RTC
    Stamp nowTime = Stamp(rtc.getYear(), rtc.getMonth(), rtc.getDate(), rtc.getHours(), rtc.getMinutes(), 0);
#else
    Stamp nowTime = Stamp(FAKE_RTC_YEAR, FAKE_RTC_MONTH, FAKE_RTC_DAY, FAKE_RTC_HOUR, FAKE_RTC_MIN, FAKE_RTC_SEC);
#endif
    lcd.print("Next feed time:");
    lcd.setCursor(0, 1);
    Datime nextFeedTime = config.getNextFeed(nowTime);
    lcd.print(nextFeedTime.day + String(".") + String(nextFeedTime.month) + String(".") + String(nextFeedTime.year) + String(" ") + String(nextFeedTime.hour) + String(":") + String(nextFeedTime.minute));
#if LOGGING
    Serial.println(String(nextFeedTime.day) + String(".") + String(nextFeedTime.month) + String(".") + String(nextFeedTime.year) + String(" ") + String(nextFeedTime.hour) + String(":") + String(nextFeedTime.minute));
#endif
  }
  else
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Clock not found");
    lcd.setCursor(0, 1);
    lcd.print("Press select");
#if LOGGING
    Serial.println("---");
#endif
  }
}

void setup()
{
  lcd.begin(16, 2);
#if LOGGING
  Serial.begin(9600);
#endif
  pinMode(A0, INPUT_PULLUP);
  config.load();
  initMenus();
  selfCheck();
  feeder.setPortionSize(config.portionSize);
  feeder.setSpeed(config.speed);
  selector.setMainMenu(menus);
  feederTimer.start();
  showNextFeedTime();
}

void loop()
{
  if (feederTimer.tick())
  {
    regularScheduleCheck();
  }

  if (selector.showMainMenuOnSelect())
  {
    showNextFeedTime();
  }
}
