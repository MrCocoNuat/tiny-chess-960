// Thanks, Technoblogy
// - http://www.technoblogy.com/show?23OS

// heavily modified to support ATtiny84 and add some new features,
// as well as optimize the hell out of anything already here

#include <Wire.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <Arduino.h>

/* Tiny Graphics Library v3 - see http://www.technoblogy.com/show?23OS

   David Johnson-Davies - www.technoblogy.com - 23rd September 2018
   ATtiny85 @ 8 MHz (internal oscillator; BOD disabled)

   CC BY 4.0
   Licensed under a Creative Commons Attribution 4.0 International license:
   http://creativecommons.org/licenses/by/4.0/
*/


// Pins
int const sda = 6;
int const scl = 4;

// Constants
int const address = 60;
int const commands = 0x00;
int const onecommand = 0x80;
int const data = 0x40;
int const onedata = 0xC0;

// OLED display **********************************************

// Character set for text - stored in program memory
// 6 columns of 8, only ascii 0x20 (space) to 0x5A (Z)
const uint8_t CHAR_MAP[59][6] PROGMEM = {
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x5F, 0x00, 0x00, 0x00 },
  { 0x00, 0x07, 0x00, 0x07, 0x00, 0x00 },
  { 0x14, 0x7F, 0x14, 0x7F, 0x14, 0x00 },
  { 0x24, 0x2A, 0x7F, 0x2A, 0x12, 0x00 },
  { 0x23, 0x13, 0x08, 0x64, 0x62, 0x00 },
  { 0x36, 0x49, 0x56, 0x20, 0x50, 0x00 },
  { 0x00, 0x08, 0x07, 0x03, 0x00, 0x00 },
  { 0x00, 0x1C, 0x22, 0x41, 0x00, 0x00 },
  { 0x00, 0x41, 0x22, 0x1C, 0x00, 0x00 },
  { 0x2A, 0x1C, 0x7F, 0x1C, 0x2A, 0x00 },
  { 0x08, 0x08, 0x3E, 0x08, 0x08, 0x00 },
  { 0x00, 0x80, 0x70, 0x30, 0x00, 0x00 },
  { 0x08, 0x08, 0x08, 0x08, 0x08, 0x00 },
  { 0x00, 0x00, 0x60, 0x60, 0x00, 0x00 },
  { 0x20, 0x10, 0x08, 0x04, 0x02, 0x00 },
  { 0x3E, 0x51, 0x49, 0x45, 0x3E, 0x00 },
  { 0x00, 0x42, 0x7F, 0x40, 0x00, 0x00 },
  { 0x72, 0x49, 0x49, 0x49, 0x46, 0x00 },
  { 0x21, 0x41, 0x49, 0x4D, 0x33, 0x00 },
  { 0x18, 0x14, 0x12, 0x7F, 0x10, 0x00 },
  { 0x27, 0x45, 0x45, 0x45, 0x39, 0x00 },
  { 0x3C, 0x4A, 0x49, 0x49, 0x31, 0x00 },
  { 0x41, 0x21, 0x11, 0x09, 0x07, 0x00 },
  { 0x36, 0x49, 0x49, 0x49, 0x36, 0x00 },
  { 0x46, 0x49, 0x49, 0x29, 0x1E, 0x00 },
  { 0x00, 0x00, 0x14, 0x00, 0x00, 0x00 },
  { 0x00, 0x40, 0x34, 0x00, 0x00, 0x00 },
  { 0x00, 0x08, 0x14, 0x22, 0x41, 0x00 },
  { 0x14, 0x14, 0x14, 0x14, 0x14, 0x00 },
  { 0x00, 0x41, 0x22, 0x14, 0x08, 0x00 },
  { 0x02, 0x01, 0x59, 0x09, 0x06, 0x00 },
  { 0x3E, 0x41, 0x5D, 0x59, 0x4E, 0x00 },
  { 0x7C, 0x12, 0x11, 0x12, 0x7C, 0x00 },
  { 0x7F, 0x49, 0x49, 0x49, 0x36, 0x00 },
  { 0x3E, 0x41, 0x41, 0x41, 0x22, 0x00 },
  { 0x7F, 0x41, 0x41, 0x41, 0x3E, 0x00 },
  { 0x7F, 0x49, 0x49, 0x49, 0x41, 0x00 },
  { 0x7F, 0x09, 0x09, 0x09, 0x01, 0x00 },
  { 0x3E, 0x41, 0x41, 0x51, 0x73, 0x00 },
  { 0x7F, 0x08, 0x08, 0x08, 0x7F, 0x00 },
  { 0x00, 0x41, 0x7F, 0x41, 0x00, 0x00 },
  { 0x20, 0x40, 0x41, 0x3F, 0x01, 0x00 },
  { 0x7F, 0x08, 0x14, 0x22, 0x41, 0x00 },
  { 0x7F, 0x40, 0x40, 0x40, 0x40, 0x00 },
  { 0x7F, 0x02, 0x1C, 0x02, 0x7F, 0x00 },
  { 0x7F, 0x04, 0x08, 0x10, 0x7F, 0x00 },
  { 0x3E, 0x41, 0x41, 0x41, 0x3E, 0x00 },
  { 0x7F, 0x09, 0x09, 0x09, 0x06, 0x00 },
  { 0x3E, 0x41, 0x51, 0x21, 0x5E, 0x00 },
  { 0x7F, 0x09, 0x19, 0x29, 0x46, 0x00 },
  { 0x26, 0x49, 0x49, 0x49, 0x32, 0x00 },
  { 0x03, 0x01, 0x7F, 0x01, 0x03, 0x00 },
  { 0x3F, 0x40, 0x40, 0x40, 0x3F, 0x00 },
  { 0x1F, 0x20, 0x40, 0x20, 0x1F, 0x00 },
  { 0x3F, 0x40, 0x38, 0x40, 0x3F, 0x00 },
  { 0x63, 0x14, 0x08, 0x14, 0x63, 0x00 },
  { 0x03, 0x04, 0x78, 0x04, 0x03, 0x00 },
  { 0x61, 0x59, 0x49, 0x4D, 0x43, 0x00 }
};

// spritesheet
// this is modified a bit from the generated monochrome-spritesheet
// the PROGMEM directive has been added
// indices have been shifted around so that the PieceType enum + allegiance bit can index directly into here,
// with non-piece sprites taking up the remaining spots (X000 and X111)

const uint8_t monochrome_sprites[16][8] PROGMEM = {
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000}, // black tile
    {0b00000000, 0b00000000, 0b00000010, 0b00101110, 0b00101110, 0b00000010, 0b00000000, 0b00000000}, // white pawn
    {0b00000000, 0b00001100, 0b00010010, 0b01001110, 0b01001110, 0b00010010, 0b00001100, 0b00000000}, // white king
    {0b00000000, 0b00000000, 0b00010010, 0b00110110, 0b01111110, 0b00111010, 0b00000000, 0b00000000}, // white knight
    {0b00000000, 0b00000000, 0b00011010, 0b00111110, 0b01001110, 0b00011010, 0b00000000, 0b00000000}, // white bishop
    {0b00000000, 0b00000000, 0b01100010, 0b00111110, 0b01111110, 0b01100010, 0b00000000, 0b00000000}, // white rook
    {0b00000000, 0b00110000, 0b00001010, 0b01101110, 0b01101110, 0b00001010, 0b00110000, 0b00000000}, // white queen
    {0b00000000, 0b00000000, 0b00000000, 0b00011000, 0b00011000, 0b00000000, 0b00000000, 0b00000000}, // possible move
    {0b00000000, 0b00100100, 0b01001000, 0b00010010, 0b00100100, 0b01001000, 0b00010010, 0b00000000}, // white tile
    {0b00000000, 0b00000011, 0b00101101, 0b01010001, 0b01010001, 0b00101101, 0b00000011, 0b00000000}, // black pawn
    {0b00001100, 0b00010011, 0b01101101, 0b10110001, 0b10110001, 0b01101101, 0b00010011, 0b00001100}, // black king
    {0b00000000, 0b00010011, 0b00101101, 0b01001001, 0b10000001, 0b01000101, 0b00111011, 0b00000000}, // black knight
    {0b00000000, 0b00011011, 0b00100101, 0b01000001, 0b10010001, 0b01100101, 0b00011011, 0b00000000}, // black bishop
    {0b00000000, 0b01100011, 0b10011101, 0b10000001, 0b01000001, 0b10011101, 0b01100011, 0b00000000}, // black rook
    {0b00110000, 0b01001011, 0b01110101, 0b10010001, 0b10010001, 0b01110101, 0b01001011, 0b00110000}, // black queen
    {0b11100111, 0b10000001, 0b10000001, 0b00000000, 0b00000000, 0b10000001, 0b10000001, 0b11100111}, // cursor
};

// Current plot position
int x0;
int y0;

// Send a single command
void command(uint8_t x) {
  Wire.write(onecommand);
  Wire.write(x);
}

void initI2C(){
  Wire.begin();
}

void initDisplay() {
  Wire.beginTransmission(address);
  Wire.write(commands);
  Wire.write(0xA1);  // Flip horizontal
  Wire.write(0xAF);  // Display on
  Wire.endTransmission();
}

void clearDisplay() {
  for (int p = 0; p < 8; p++) {
    Wire.beginTransmission(address);
    command(0xB0 + p);
    Wire.endTransmission();
    for (int q = 0; q < 8; q++) {
      Wire.beginTransmission(address);
      Wire.write(data);
      for (int i = 0; i < 20; i++) Wire.write(0);
      Wire.endTransmission();
    }
  }
}

void setupDisplay(){
    initI2C();
    clearDisplay();
    initDisplay();
}

// Move current plot position to x,y
void moveTo(int x, int y) {
  x0 = x;
  y0 = y;
}

void plotPoint (int x, int y) {
  Wire.beginTransmission(address);
  command(0x00 + ((x + 2) & 0x0F));        // Column low nibble
  command(0x10 + ((x + 2)>>4));            // Column high nibble
  command(0xB0 + (y >> 3));                // Page
  command(0xE0);                           // Read modify write
  Wire.write(onedata);
  Wire.endTransmission();
  Wire.requestFrom(address, 2);
  Wire.read();                            // Dummy read
  int j = Wire.read();
  Wire.beginTransmission(address);
  Wire.write(onedata);
  Wire.write((1<<(y & 0x07)) | j);
  command(0xEE);                           // Cancel read modify write
  Wire.endTransmission();
}

uint8_t reverseByte(uint8_t x) {
  x = ((x >> 1) & 0x55) | ((x << 1) & 0xaa);
  x = ((x >> 2) & 0x33) | ((x << 2) & 0xcc);
  x = ((x >> 4) & 0x0f) | ((x << 4) & 0xf0);
  return x;
}

// private method - write bytes of a char, assuming caller already placed display in ReadModifyWrite mode
writeChar(int c) {
  for (int col = 0; col < 6; col++) {
    Wire.write(onedata);
    Wire.write(
      reverseByte( // i'd rather reverse the sprite maps instead
          pgm_read_byte(&CHAR_MAP[c - 32][col])
      )
    );
  }
}

// Plot an ASCII character with bottom left corner at x in vertical page
// only supports y divisible by 8, hence page instead of y
void plotChar(int c, int x, int page) {
  Wire.beginTransmission(address);
  command(0xB0 + page);
  // column is automatically incremented on display memory accessses so no need to keep re-setting it
  command(0x00 + ((x + 2) & 0x0F));  // initial Column low nibble
  command(0x10 + ((x + 2) >> 4));    // initial Column high nibble
  command(0xE0);  // enter read modify write
  writeChar(c);
  command(0xEE);  // exit read modify write
  Wire.endTransmission();
}

// Plot text starting at the current plot position - really really fast, a full screen in 0.2 seconds
void plotText(PGM_P s) {
  int p = (int) s;
  Wire.beginTransmission(address);
  command(0xB0 + (y0 >> 3));
  // column is automatically incremented on display memory writes so no need to keep re-setting it!
  command(0x00 + ((x0 + 2) & 0x0F));  // initial Column low nibble
  command(0x10 + ((x0 + 2) >> 4));    // initial Column high nibble
  command(0xE0); // enter read modify write
  while (true) {
    char c = pgm_read_byte(p++);
    if (c == 0) break;
    Wire.endTransmission(); // microcontroller has a really small i2c buffer! so flush it per character
    Wire.beginTransmission(address);
    writeChar(c);
  }
  command(0xEE);  // exit read modify write
  Wire.endTransmission();
}

// Plot a 8*8 sprite at the current plot position - really really fast. The sprite should be defined as the text glyphs here are, columns of 8 bits for lit pixels
void plotSprite(uint8_t spriteIndex){
  Wire.beginTransmission(address);
  command(0xB0 + (y0 >> 3));
  // column is automatically incremented on display memory writes so no need to keep re-setting it!
  command(0x00 + ((x0 + 2) & 0x0F));  // initial Column low nibble
  command(0x10 + ((x0 + 2) >> 4));    // initial Column high nibble
  command(0xE0); // enter read modify write

  Wire.endTransmission(); // microcontroller has a really small i2c buffer! so flush it per character
  Wire.beginTransmission(address);
  for (int col = 0; col < 8; col++) {
    Wire.write(onedata);
    Wire.write(pgm_read_byte(&monochrome_sprites[spriteIndex][col]));
  }

  command(0xEE);  // exit read modify write
  Wire.endTransmission();
}

void invert(){
  Wire.beginTransmission(address);
  command(0xA7); // inverse display output
  Wire.endTransmission();
}

void unInvert(){
  Wire.beginTransmission(address);
  command(0xA6);
  Wire.endTransmission();
}

// the above 2 in a row
void blink(){
  invert();
  delay(100);
  unInvert();
}


// these disregard the "invert" state
void forceDisplayAllOff(){
  Wire.beginTransmission(address);
  command(0xAE); // turn display off - overrides any other command effect
  Wire.endTransmission();
}

void forceDisplayAllOn(){
  Wire.beginTransmission(address);
  command(0xA5); // force all segments on
  Wire.endTransmission();
}

void resetForceDisplay(){
  Wire.beginTransmission(address);
  command(0xA4); // undo force all segments on
  command(0xAF); // turn display back on
  Wire.endTransmission();
}
// contrast but for OLEDs
// default is 1 << 7
void setBrightness(uint8_t brightness){
  Wire.beginTransmission(address);
  command(0x81); // enable contrast control mode
  command(brightness); // send it, and exit contrast control mode
  Wire.endTransmission();
}

void resetBrightness(){
  setBrightness(1 << 7);
}

// max lines = 63 !!
// loops the display ram across the display with this offset
void setScroll(uint8_t lines){
  Wire.beginTransmission(address);
  command(0x40 | (lines & 0x3F));
  Wire.endTransmission();
}

void resetScroll(){
  setScroll(0);
}