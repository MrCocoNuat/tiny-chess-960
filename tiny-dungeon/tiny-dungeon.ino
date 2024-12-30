#include "sh1106.hh"
#include "board.hh"
#include "debug-modes.hh"

// ATtiny84
// 8MHz (but really OSCCAL = 0xFF) operation

uint8_t displayBuffer[][]

void setup(){
  setupBoard();
  setupDisplay();
}

void loop() {
  debugDisplayRefreshRate();
}



