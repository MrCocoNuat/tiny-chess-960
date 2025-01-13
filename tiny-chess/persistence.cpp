// let's have fun with 512B of persistent storage!

#include <EEPROM.h>
#include "types.hh"

void putInAddress0(uint8_t something){
  EEPROM.update(0, something); // don't mess up address 0 too much for testing. 
}

uint8_t getFromAddress0(){
  uint8_t value;
  EEPROM.get(0, value);
  return value;
}