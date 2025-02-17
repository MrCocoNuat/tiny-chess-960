#include "sh1106.hh"
#include "board.hh"
#include "debug-modes.hh"
#include "main.hh"
#include "menu.hh"

// ATtiny84
// 8MHz (but really OSCCAL = 0xFF) operation

void setup(){
  setupBoard();
  setupDisplay();

  uint16_t gameSeed = doMenu() & ~(FLAG_NEW_GAME_SEED);
  clearDisplay();

  initialize(gameSeed);
  doIt();
}

void loop() {}



