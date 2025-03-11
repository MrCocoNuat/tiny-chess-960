#pragma once

#include <avr/pgmspace.h>

void initI2C();
void command(uint8_t x);
void plotPoint (int x, int y);
void initDisplay();
void invert();
void unInvert();
void blink();
void clearDisplay();
void setupDisplay();
void moveTo(int x, int y);
uint8_t reverseByte(uint8_t x);
void plotChar(int c, int x, int page);
void plotText(PGM_P s);
void plotSprite(uint8_t spriteIndex);
void forceDisplayAllOff();
void forceDisplayAllOn();
void resetForceDisplay();
void setBrightness(uint8_t brightness);
void resetBrightness();
void setScroll(uint8_t lines);
void resetScroll();