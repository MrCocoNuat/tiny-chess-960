#include "sh1106.hh"
#include "board.hh"
#include <avr/pgmspace.h>
#include <Arduino.h>

void debugJoysticks(){
  uint8_t joystickPlotX[] = {82, 94, 106, 94, 22, 34, 46, 34};
  uint8_t joystickPlotY[] = {23, 31, 23, 15, 24, 32, 24, 16};

  volatile uint8_t prevJoystickState = 0b00000000;
  volatile uint8_t joystickState = 0b00000000;

  MoveTo(24,48);
  PlotText(PSTR("JOYSTICK  TEST"));
  for (int i = 7; i >= 0; i--) {
    PlotChar('0', joystickPlotX[i], joystickPlotY[i] >> 3);
  }
  for (;;) {
    prevJoystickState = joystickState;
    joystickState = getJoystickState();
    for (int i = 7; i >= 0; i--) {
      if ((joystickState & (1 << i)) ^ (prevJoystickState & (1 << i))) { // there is a difference in this joystick input!
        PlotChar((joystickState & (1 << i)) ? '1' : '0', joystickPlotX[i], joystickPlotY[i] >> 3);
      }
    }
  }
}


void debugDisplay(){
  uint8_t joystickPlotX[] = {82, 94, 106, 94, 22, 34, 46, 34};
  uint8_t joystickPlotY[] = {23, 31, 23, 15, 24, 32, 24, 16};

  uint8_t brightness = 1 << 5; // in steps of 4
  uint8_t scroll = 0;

  // light up the corners
  PlotPoint(0,0);
  PlotPoint(127,63);
  PlotPoint(0,63);
  PlotPoint(127,0);

  MoveTo(30,48);
  PlotText(PSTR("DISPLAY TEST"));
  MoveTo(6,40);
  PlotText(PSTR("RIGHT SCROLL/BRIGHT"));
  MoveTo(12,32);
  PlotText(PSTR("LEFT INVERT/FORCE"));

  for (;;){
    uint8_t joystickState = getJoystickState();
    if (joystickState == 0){
      continue; // no delay
    }
    if (joystickState & (1 << STICK_RL)){ // brightness down
      setBrightness(--brightness << 2);
    }
    if (joystickState & (1 << STICK_RU)){ // scroll up
      setScroll(--scroll);
    }
    if (joystickState & (1 << STICK_RR)){ // brightness up
      setBrightness(++brightness << 2);
    }
    if (joystickState & (1 << STICK_RD)){ // scroll down
      setScroll(++scroll);
    }
    if (joystickState & (1 << STICK_LL)){ // normal output
      resetForceDisplay();
      unInvert();
    }
    if (joystickState & (1 << STICK_LU)){ // force on
      resetForceDisplay();
      forceDisplayAllOn();
    }
    if (joystickState & (1 << STICK_LR)){ // invert output
      resetForceDisplay();
      invert();
    }
    if (joystickState & (1 << STICK_LD)){ // force off
      forceDisplayAllOff();
    }
    delay(50);
  }
}

void debugTone(){
    int8_t tone = 24;

    MoveTo(30,48);
    PlotText(PSTR("SPEAKER TEST"));
    MoveTo(9,40);
    PlotText(PSTR("RIGHT STICK L MUTE"));
    MoveTo(18,32);
    PlotText(PSTR("U/D CHANGE TONE"));

    for (;;){
      uint8_t joystickState = getJoystickState();
      if (joystickState == 0){
        continue; // no delay
      }
      if (joystickState & (1 << STICK_RL)){ // mute
        playTone(-1);
      }
      if (joystickState & (1 << STICK_RU)){ // unmute tone up
        playTone(++tone);
      }
      if (joystickState & (1 << STICK_RR)){ // unmute without changing tone
        playTone(tone);
      }
      if (joystickState & (1 << STICK_RD)){ // unmute tone down
        playTone(--tone);
      }
      delay(200);
    }
};