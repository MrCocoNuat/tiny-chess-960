#include <avr/io.h>
#include <avr/interrupt.h>
#include "board.hh"

void setupBoard() {
  OSCCAL = 0xFF; // go as fast as possible

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

  // Timer 1 is being used for tone playback
  // - use timer 0 to count units of ... 4 milliseconds, I guess
  // this is used for delay and UI purposes (no using the horribly expensive general arduino version)
  // oh no, slowest timer0 can be is /1024... so need artificial timing? but even 30Hz interrupts are sooooo costly especially in a context where audio is being played

  // /1024 prescaler on timer 0
  TCCR0B = 0b00000101;

  // output on OC1B
  TCCR1A = (1 << COM1B0);
  // CTC (clear on OCR1A)
  TCCR1B = (1 << WGM12);

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

const uint8_t STICK_ALL_RIGHT = 0x0F;
const uint8_t STICK_ALL_LEFT = 0xF0;

void delayLite(uint8_t timeUnits){ // aprox 100uS
  uint8_t target = TCNT0 + timeUnits;
  // busywait - not like there's anything better to do
  while (TCNT0 != target);
}

// use the prescaler of timer 1: realistically we only need /8 -> 1MHz timer ticks, 64Hz timer overflows
// Offer 4 octaves, because the decreasing precision at OCR1A ~= 0xFFFF / 0b00001000 is still OK. But caller can ask for higher too
// let C1=65Hz be octave 1, tone 0, and just use shifts to go up octaves
// These values are in geometric progression.
const uint16_t timerTable[] = {
  15288, // C
  14430, // C#
  13620, // D
  12856, // Db
  12134, // E
  11453, // F
  10810, // F#
  10204, // G
  9631,  // Ab
  9090,  // A
  8580,  // Bb
  8098   // B
};
// "reasonable" output for the speaker ranges from:
// tone 21 = A2 to tone 81 = A7, outside of that the speaker sounds pretty bad...
void playTone(int8_t tone){
  // tone < 0 means stop output, so check that sign bit
  // in TCCR1B[CS12:CS10], 010 is prescaler /8 and 000 is stop timer
  TCCR1B = (TCCR1B & 0b11111000) | (!(tone & (1 << 7)) << CS11);
  // just set this anyway, no point adding a branch
  OCR1A = OCR1B = timerTable[tone % 12] >> (tone / 12);
}
