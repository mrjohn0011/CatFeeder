#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal.h>

const int BUTTON_PIN = A0;

enum Button {
  NO_BUTTON = -1,
  RIGHT_BUTTON = 0,
  UP_BUTTON = 1,
  DOWN_BUTTON = 2,
  LEFT_BUTTON = 3,
  SELECT_BUTTON = 4
};

unsigned short selectedIndex = 0;
const int maxIndex = 2;
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

struct Menu {
  const char* name;
  void (*processFunction)();
};

void selectDate() {
  lcd.clear();
  lcd.setCursor(0, 0);
}

void processDate() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Date!");
}

void processDuration() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Duration!");
}

Menu menus[] = {
  {"Date", processDate },       
  {"Duration", processDuration}
};

void displayMenu(Menu menu) {
  lcd.clear();
  lcd.setCursor(0, 0);
  if (selectedIndex > 0) {
    lcd.print("< ");
  }
  lcd.print(menu.name);
  if (selectedIndex < maxIndex - 1){
    lcd.print(" >");
  }
}

Button readButton() {
  int val = analogRead(BUTTON_PIN);
  if (val < 60) {
    return RIGHT_BUTTON;
  } else if (val < 200) {
    return UP_BUTTON;
  } else if (val < 400) {
    return DOWN_BUTTON;
  } else if (val < 600) {
    return LEFT_BUTTON;
  } else if (val < 800) {
    return SELECT_BUTTON;
  }
  return NO_BUTTON;
}

void handleButtonPress() {
  int button = readButton();
  switch (button) {
    case RIGHT_BUTTON:
      if (selectedIndex < maxIndex - 1) selectedIndex++;
      displayMenu(menus[selectedIndex]);
      break;
    case LEFT_BUTTON:
      if (selectedIndex > 0) selectedIndex--;
      displayMenu(menus[selectedIndex]);
      break;
    case SELECT_BUTTON:
      menus[selectedIndex].processFunction();
      break;
    default:
      break;
  }

  delay(0);
}

void setup() {
  lcd.begin(16, 2);
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  displayMenu(menus[0]);
}

void loop() {
  handleButtonPress();
}
