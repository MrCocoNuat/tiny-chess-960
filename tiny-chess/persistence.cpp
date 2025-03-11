// let's have fun with 512B of persistent storage!

#include <EEPROM.h>
#include "types.hh"

// there are 512B, or 256 blocks of eeprom at 2B each
void put(uint8_t address, uint16_t content){
  EEPROM.update(address << 1, content >> 8);
  EEPROM.update((address << 1) + 1, content & 0xFF);
}

uint16_t get(uint8_t address){
  return (EEPROM.read(address << 1) << 8) | EEPROM.read((address << 1) + 1);
}
