#pragma once

#include <avr/pgmspace.h>

void InitI2C();
void Single(uint8_t x);
void InitDisplay();
void invert();
void unInvert();
void blink();
void ClearDisplay();
void setupDisplay();
void PlotPoint(int x, int y);
void MoveTo(int x, int y);
void DrawTo(int x, int y);
uint8_t ReverseByte(uint8_t x);
void PlotChar(int c, int x, int page);
void PlotText(PGM_P s);
void forceDisplayAllOff();
void forceDisplayAllOn();
void resetForceDisplay();
void setBrightness(uint8_t brightness);
void resetBrightness();
void setScroll(uint8_t lines);
void resetScroll();