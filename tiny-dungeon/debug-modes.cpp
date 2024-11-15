#include "sh1106.hh"
#include "board.hh"
#include <avr/pgmspace.h>
#include <Arduino.h>
#include "math.hh"

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
}

// just a speed test for IPC and so on
// from these results, about 300 C instructions can be completed "instantaneously" in user's perspective
void debugCpu(){
  MoveTo(0,56);
  PlotText(PSTR("COUNT RANDS OVER 128"));
  MoveTo(36,48);
  PlotText(PSTR("LEFT STICK"));
  MoveTo(0,40);
  PlotText(PSTR("L 1K R 3K U 10K D 30K"));

  for (;;){
    uint8_t joystickState = getJoystickState();
    if ((joystickState & (1 << STICK_LL | 1 << STICK_LR | 1 << STICK_LU | 1 << STICK_LD)) == 0){
      continue; // no delay
    }

    MoveTo(54,16);
    PlotText(PSTR("RUN"));
    uint32_t counts = 0;
    if (joystickState & (1 << STICK_LL)){
      counts = 1000; // 400msec
    }
    if (joystickState & (1 << STICK_LR)){
      counts = 3000; // 1sec
    }
    if (joystickState & (1 << STICK_LU)){
      counts = 10000; // 4sec
    }
    if (joystickState & (1 << STICK_LD)){
      counts = 30000; // 12sec
    }

    for (; counts > 0;){
      if (nextByte() > 1 << 7){
        counts--;
      }
    }

    MoveTo(54,16);
    PlotText(PSTR("OK "));
  }
}

// how deep can you go? stack frame must contain a 4B param so this doesn't take forever
void testStack(uint32_t x){
  if (x == 0xFF){
    return; // should never make it, but convinces the compiler to not optimize second recursive call as dead code
  } 

  MoveTo(x,0);
  PlotText(PSTR("A"));
  while (getJoystickState() == 0){}

  testStack(x+1); // will overlap text but that is fine
  testStack(x+1); // kill tail call optimization, this is never called
}

void debugStack(){
  MoveTo(18, 48);
  PlotText(PSTR("STACK SIZE TEST"));
  MoveTo(9, 40);
  PlotText(PSTR("PRESS ANY FOR AN A"));

  testStack(0);
}

// 70 frames with 4B, 110 with 2B. So 110(2+x) = 70(4+x) => bare stack frame is 1.5B -> 380 used in testStack 
