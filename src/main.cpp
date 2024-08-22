#include <MenuSelector/MenuSelector.h>
#include <Settings/Settings.h>
#include <Feeder/Feeder.h>
#include <MicroDS3231.h>

#define MENU_COUNT 2
#define SCHEDULE_COUNT 4

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
MenuSelector selector(&lcd, 0, MENU_COUNT + SCHEDULE_COUNT);

Feeder feeder(50, 3, 2);
TimerMs feederTimer(20000, true, false);
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

Menu menus[MENU_COUNT + SCHEDULE_COUNT];

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
}

bool checkRTC()
{
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

  return true;
}

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
  initMenus();

  selector.setMainMenu(menus);
  feederTimer.start();
  lcd.clear();
  lcd.print("Press select");
}

void loop()
{
  if (feederTimer.tick())
  {
    Serial.println("Regular schedule checking...");
    if (checkRTC())
    {
      for (int i = 0; i < SCHEDULE_COUNT; i++)
      {
        if (config.portions[i].isTimeToFeed(Stamp(rtc.getYear(), rtc.getMonth(), rtc.getDate(), rtc.getHours(), rtc.getMinutes(), 0)))
        {
          feeder.feed(config.portions[i].getAmount());
          config.portions[i].setLastFeed(Datime(rtc.getYear(), rtc.getMonth(), rtc.getDate(), rtc.getHours(), rtc.getMinutes(), 0));
          Serial.print("Schedule " + String(i + 1) + " fed: ");
          Serial.println(config.portions[i].getAmount());
          break;
        }
      }
    }
  }

  selector.waitForSelect();
}
