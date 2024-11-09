#include <avr/io.h>
#include <avr/interrupt.h>

void setupBoard() {
  // pins: <L/R><L/R/U/D> are left/right 'joystick' directions
  //     ------------------
  //    -| VCC        GND |-
  // LR -| B0/10     A0/0 |- RL
  // LU -| B1/9      A1/1 |- RU
  //    -| RST       A2/2 |- RR
  // LL -| B2/8      A3/3 |- RD
  // LD -| A7/7    A4/SCL |-
  //    -| A6/SDA A5/OC1B |- Buzzer
  //     ------------------
  // thus the data directions are:
  DDRA = (1 << PORTA4) | (1 << PORTA5) | (1 << PORTA6);
  DDRB = 0;
  // pull up on the joystick pins
  PORTA |= (1 << PORTA7) | (1 << PORTA3) | (1 << PORTA2) | (1 << PORTA1) | (1 << PORTA0);
  PORTB |= (1 << PORTB2) | (1 << PORTB1) | (1 << PORTB0);

  // output on OC1B
  TCCR1A = (1 << COM1B0);
  // CTC (clear on OCR1A), /8 prescaler
  TCCR1B = (1 << WGM12) | (1 << CS10);
  OCR1A = 4000;  // for CTC
  OCR1B = 4000;  // about 2kHz out

  sei();
}


// cram the 8 bits (high = direction pushed) into 1 return byte
// with order: msb- LD LR LU LL RD RR RU RL -lsb
uint8_t getJoystickState() {
  // remember that joystick pins are active low so invert too to get active directions
  //   and that A4, A6 are SPI pins, and A5 is the buzzer! So mask those out of pinA!!
  uint8_t pinA = PINA & ~(1 << PORTA6 | 1 << PORTA5 | 1 << PORTA4) , pinB = PINB;
  return ~(pinA | ((pinB & (1 << PORTB0)) << 6) | ((pinB & (1 << PORTB1)) << 4) | ((pinB & (1 << PORTB2)) << 2));
}

void playTone(){
  // use the prescaler of timer 1:
  // /1, /8, /64, /256, /1024 spans 10 octaves, but for convenience we don't use /1024 (because that is way too little granularity) to keep 8* ratios
  // the actually useful audible range for most buzzers is 100Hz to 8000Hz, so 6 octaves is plenty already. Offer all 8
  // let A2=110Hz be octave 2, tone 0. A8=7040Hz be octave 8, tone 0 and the maximum
}
