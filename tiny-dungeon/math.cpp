#include <Arduino.h>

// simple LSFR, 2^32 - 1 period is good enough
static uint32_t randomState = 0b10000000000000000000000000000001;
uint8_t nextByte(){
  uint8_t nextByte = 0;
  for (uint8_t i = 8; i > 0; i--){
    // Feedback bit (tap positions are defined by the polynomial x^32 + x^22 + x^2 + x + 1)
    uint32_t feedback = ((randomState >> 31) ^ (randomState >> 21) ^ (randomState >> 1) ^ randomState) & 0x1;
    // Shift the register left by 1 and add the feedback at the rightmost bit
    randomState = (randomState << 1) | feedback;
    nextByte = (nextByte << 1) | feedback;
  }
  return nextByte;
}