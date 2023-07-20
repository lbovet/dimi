#include <Arduino.h>
#include <EEPROM.h>

#define driver PIN4
#define led PIN3
#define BOOT_TIME 6000 * 2
#define SELECT_TIME 5000 * 2
#define STATE 0
#define MODE 1
#define WRITE

enum State {
  BOOT,
  NORMAL,
  SELECT
};

byte state = BOOT;
byte mode = 3;
uint32_t chrono = 0;

void update() {
  switch (mode)
  {
  case 0:
    analogWrite(driver, 10);
    break;
  case 1:
    analogWrite(driver, 20);
    break;
  case 2:
    analogWrite(driver, 60);
    break;
  default:
    analogWrite(driver, 128);
    break;
  }
}

void select() {
  mode = 0;
  state = SELECT;
}

void setup()
{
  pinMode(driver, OUTPUT);
  pinMode(led, OUTPUT);
  state = EEPROM.read(STATE);
  if(state == SELECT) {
    select();
    #ifdef WRITE
    EEPROM.write(STATE, BOOT);
    #endif
  } else {
    state = BOOT;
    mode = EEPROM.read(MODE);
    digitalWrite(led, HIGH);
    update();
    #ifdef WRITE
    EEPROM.write(STATE, SELECT);
    #endif
  }
}

void loop()
{
  switch (state)
  {
  case BOOT:
    if(millis() > BOOT_TIME) {
      state = NORMAL;
      digitalWrite(led, LOW);
      #ifdef WRITE
      EEPROM.write(STATE, state);
      #endif
    }
    break;
  case SELECT:
    if(millis() > chrono) {
      chrono = millis() + SELECT_TIME;
      if(mode < 4) {
        update();
        #ifdef WRITE
        EEPROM.write(MODE, mode);
        #endif
        mode++;
      } else {
        state = NORMAL;
      }
    }
    break;
  case NORMAL:
    break;
  }
  delay(20);
}
