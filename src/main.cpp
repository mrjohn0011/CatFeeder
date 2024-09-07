#include <MenuSelector/MenuSelector.h>
#include <Settings/Settings.h>
#include <Feeder/Feeder.h>
#include <MicroDS3231.h>

#define MENU_COUNT 4
#define SCHEDULE_COUNT 4
#define USE_REAL_RTC false

#define FAKE_RTC_YEAR 2024
#define FAKE_RTC_MONTH 9
#define FAKE_RTC_DAY 1
#define FAKE_RTC_HOUR 10
#define FAKE_RTC_MIN 30
#define FAKE_RTC_SEC 0

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
  Serial.print("Setting new current time: ");
  Serial.println(nowTime.toString());
#if USE_REAL_RTC
  rtc.setTime(dt.second, dt.minute, dt.hour, dt.day, dt.month, dt.year);
  Serial.print("Clock updated to: ");
  Serial.print(rtc.getDateString());
  Serial.print(" ");
  Serial.println(rtc.getTimeString());
#endif
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

Menu menus[MENU_COUNT + SCHEDULE_COUNT];

void setupSpeed()
{
  int speed = selector.selectNumber(config.speed, 1, 10);
  feeder.setSpeed(speed);
  config.save();
  menus[6].defaultValue = String(speed);
  Serial.print("Speed value set to: ");
  Serial.println(speed);
}

void setupPortionSize()
{
  int portionSize = selector.selectNumber(config.portionSize, 1, 16);
  feeder.setPortionSize(portionSize);
  config.save();
  menus[7].defaultValue = String(portionSize);
  Serial.print("Portion size: ");
  Serial.println(portionSize);
}

void scheduleSetter(uint8_t i)
{
  config.portions[i] = selector.selectPortion(config.portions[i]);
  Serial.print("Schedule " + String(i + 1) + " set to: ");
  Serial.println(config.portions[i].toString());
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
#endif
  Serial.println("Self check done");
}

void regularScheduleCheck()
{
  Serial.println("Regular schedule checking...");
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
        Serial.print("Schedule " + String(i + 1) + " fed: ");
        Serial.println(config.portions[i].getAmount());
        needSave = true;
        break;
      }
    }

    if (needSave)
    {
      config.save();
    }
  }
}

void showNextFeedTime()
{
  Serial.print("Showing next feed time: ");
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
    Serial.println(String(nextFeedTime.day) + String(".") + String(nextFeedTime.month) + String(".") + String(nextFeedTime.year) + String(" ") + String(nextFeedTime.hour) + String(":") + String(nextFeedTime.minute));
  }
  else
  {
    lcd.print("Clock not found");
    lcd.setCursor(0, 1);
    lcd.print("Press select");
    Serial.println("---");
  }
}

void setup()
{
  lcd.begin(16, 2);
  Serial.begin(9600);
  pinMode(A0, INPUT_PULLUP);
  selfCheck();
  config.load();
  initMenus();
  feeder.setPortionSize(config.portionSize);
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
