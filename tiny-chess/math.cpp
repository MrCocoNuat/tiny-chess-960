#include <Arduino.h>

// for replaying from a saved seed
uint32_t shift8(uint32_t from){
  for (uint8_t i = 8; i > 0; i--){
    // Feedback bit (tap positions are defined by the polynomial x^32 + x^22 + x^2 + x + 1)
    uint32_t feedback = ((from >> 31) ^ (from >> 21) ^ (from >> 1) ^ from) & 0x1;
    // Shift the register left by 1 and add the feedback at the rightmost bit
    from = (from << 1) | feedback;
  }
  return from;
}

// simple LSFR, 2^32 - 1 period is good enough
// advances the "global" RNG
static uint32_t randomState = 0b10000000000000000000000000000001;
uint8_t nextByte(){
  randomState = shift8(randomState);
  return randomState & 0xFF;
}

uint8_t norm(int8_t x){
  return (x > 0)? x : -x;
}
