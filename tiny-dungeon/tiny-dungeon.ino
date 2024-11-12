#include "sh1106.hh"
#include "board.hh"
#include "debug-modes.hh"

// ATtiny84
// 8MHz operation

void setup(){
  setupBoard();
  setupDisplay();
}

void loop() {
  debugCpu();
}



