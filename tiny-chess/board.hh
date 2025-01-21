#pragma once

void setupBoard();

typedef enum {
  STICK_RL,
  STICK_RU,
  STICK_RR,
  STICK_RD,
  STICK_LL,
  STICK_LU,
  STICK_LR,
  STICK_LD
} Joystick;
uint8_t getJoystickState();

extern uint8_t STICK_ALL_LEFT;
extern uint8_t STICK_ALL_RIGHT;

void delayLite(uint8_t timeUnits);

void playTone(int8_t tone);


