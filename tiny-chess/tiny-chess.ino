#include "sh1106.hh"
#include "board.hh"
#include "debug-modes.hh"
#include "chess.hh"

// ATtiny84
// 8MHz (but really OSCCAL = 0xFF) operation

void setup(){
  setupBoard();
  setupDisplay();

  fillInOpeningPositions(0);
  blitBoard();
  doIt();
}

void loop() {}



