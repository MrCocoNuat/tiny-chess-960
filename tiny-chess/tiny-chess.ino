#include "sh1106.hh"
#include "board.hh"
#include "debug-modes.hh"
#include "main.hh"
#include "menu.hh"
#include "save.hh"

// ATtiny84
// 8MHz (but really OSCCAL = 0xFF) operation

void setup(){
  setupBoard();
  setupDisplay();

  uint16_t compositeSeed = doMenu();
  clearDisplay();
  
  if (compositeSeed & MASK_CONTINUE){
    // continue game
    compositeSeed = getSeed();
  } else {
    // new game
    saveSeed(compositeSeed);
    saveTurnCount(0);
  }

  initialize(compositeSeed);
  doIt(getTurnCount());
}

void loop() {}

// improvements:
// flesh out the continue option as well as save/get seed, new game resets turn count
// history review mode to scroll the turns back
// AI to play black
// AI to play white
// uh oh, quickly coming up on 8kB
// bugs:
// on continue, mate is not rechecked. So just clear the save
