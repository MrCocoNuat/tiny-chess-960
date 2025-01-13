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

void playTone(int8_t tone);


