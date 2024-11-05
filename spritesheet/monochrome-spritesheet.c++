#include <cstdint>

static const uint8_t monochrome_sprites[64][8] = {
    {0b10101010, 0b11101010, 0b10101110, 0b10111011, 0b10101010, 0b11101110, 0b10101011, 0b10111010},
    {0b10101010, 0b10101110, 0b11101011, 0b10111010, 0b10101110, 0b10101010, 0b10111011, 0b11101010},
    {0b10101010, 0b10111111, 0b01001010, 0b01000000, 0b01000000, 0b01000100, 0b10111111, 0b10101010},
    {0b00000000, 0b00001111, 0b00010101, 0b00010101, 0b00010101, 0b00010101, 0b00001111, 0b00000000},
    {0b00000000, 0b00000111, 0b00110000, 0b00100000, 0b00101010, 0b00111000, 0b00000000, 0b00000000},
    {0b00000000, 0b00001100, 0b00000111, 0b00110000, 0b00001110, 0b00000000, 0b00011111, 0b00110000},
    {0b00000000, 0b00110000, 0b00011110, 0b00000000, 0b00111111, 0b01100000, 0b00001110, 0b00010000},
    {0b00000000, 0b00010010, 0b00001100, 0b00010100, 0b00101010, 0b01010000, 0b01100000, 0b00000000},
    {0b00000000, 0b00000000, 0b00000100, 0b00001000, 0b00010000, 0b00110000, 0b01100000, 0b00000000},
    {0b00000000, 0b00000100, 0b00101110, 0b01111010, 0b01110010, 0b00101110, 0b00000100, 0b00000000},
    {0b00000000, 0b00000010, 0b00111110, 0b01000010, 0b01000010, 0b01111100, 0b01000000, 0b00000000},
    {0b00000000, 0b00000010, 0b00000110, 0b00001100, 0b01111000, 0b01010000, 0b01110000, 0b00000000},
    {0b00100000, 0b01010000, 0b10011100, 0b01000100, 0b01000100, 0b10011100, 0b01010000, 0b00110000},
    {0b00111100, 0b01100110, 0b11110111, 0b10011101, 0b10111001, 0b11101111, 0b01100110, 0b00111100},
    {0b11111111, 0b10100001, 0b10111111, 0b10010001, 0b10011111, 0b10001001, 0b10001111, 0b11111111},
    {0b01111111, 0b01001001, 0b01111101, 0b00100101, 0b00111111, 0b00010011, 0b00011111, 0b00001001},
    {0b00100000, 0b11110000, 0b00101001, 0b01001110, 0b01111010, 0b00101111, 0b00000100, 0b00001110},
    {0b00000000, 0b00000100, 0b00111001, 0b00011110, 0b00110111, 0b00000110, 0b00001000, 0b00000000},
    {0b00000000, 0b00010000, 0b00110110, 0b00011101, 0b00001001, 0b00000010, 0b00000100, 0b00000000},
    {0b00000000, 0b00010101, 0b00101110, 0b00001110, 0b01001110, 0b01011101, 0b00110100, 0b00000000},
    {0b00000000, 0b00010000, 0b00110111, 0b01111110, 0b00111111, 0b01000101, 0b00011000, 0b00000000},
    {0b00000110, 0b00011111, 0b00111111, 0b00111111, 0b00100111, 0b00110111, 0b00011111, 0b00000110},
    {0b00000000, 0b00000000, 0b01010100, 0b01110101, 0b01011110, 0b00101011, 0b00001000, 0b00000000},
    {0b00000000, 0b00001000, 0b00110111, 0b01001101, 0b01001011, 0b00110111, 0b00001000, 0b00000000},
    {0b00000000, 0b01110000, 0b00111000, 0b00001110, 0b00001100, 0b00111010, 0b01110000, 0b00000000},
    {0b00000000, 0b00110000, 0b00011111, 0b01110100, 0b01111110, 0b00011011, 0b11110000, 0b11000000},
    {0b00000000, 0b00100101, 0b01110111, 0b00011110, 0b01111110, 0b00100101, 0b01001000, 0b00000000},
    {0b00100100, 0b01101001, 0b01101101, 0b01001110, 0b01001110, 0b01011101, 0b00110100, 0b00000000},
    {0b10111010, 0b01010100, 0b01000100, 0b00101100, 0b01011010, 0b01001010, 0b00101000, 0b00000000},
    {0b00000101, 0b01111011, 0b10000110, 0b10110110, 0b10010111, 0b11000110, 0b01111010, 0b00000101},
    {0b00000000, 0b00111110, 0b01010101, 0b01111101, 0b01000111, 0b01011001, 0b00110110, 0b00111110},
    {0b00000000, 0b00100111, 0b01110101, 0b01100101, 0b01010101, 0b01001101, 0b00111111, 0b00000000},
    {0b01000110, 0b00011111, 0b10110111, 0b00100111, 0b00111111, 0b10111111, 0b00011111, 0b01000110},
    {0b00000000, 0b10111010, 0b01101100, 0b01111100, 0b00111100, 0b01011010, 0b01001010, 0b00101000},
    {0b00000000, 0b00000000, 0b00010000, 0b00000100, 0b00000000, 0b00100000, 0b00000010, 0b00000000},
    {0b00000000, 0b00100001, 0b00000000, 0b00000100, 0b01000000, 0b00010000, 0b00000010, 0b00000000},
    {0b00000000, 0b00000000, 0b00000000, 0b00000010, 0b00000100, 0b00000010, 0b00000100, 0b00000000},
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000100, 0b00000010, 0b00000100, 0b00000000},
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000010, 0b00000100, 0b00000000},
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000100, 0b00000000},
    {0b00001000, 0b01111000, 0b01111000, 0b01000000, 0b00010100, 0b00001000, 0b00010100, 0b00000000},
    {0b00001000, 0b01111000, 0b01111000, 0b01000000, 0b00010100, 0b00010000, 0b00011100, 0b00000000},
    {0b00001000, 0b01111000, 0b01111000, 0b01000000, 0b00011010, 0b00000000, 0b00110100, 0b00000000},
    {0b00001000, 0b01111000, 0b01111000, 0b01000000, 0b00011110, 0b00110010, 0b00011110, 0b00000000},
    {0b00000110, 0b00011001, 0b00011001, 0b00000110, 0b01110000, 0b00111000, 0b01110000, 0b00000000},
    {0b00000110, 0b00011001, 0b00011001, 0b00000110, 0b00100000, 0b01110000, 0b00100000, 0b00000000},
    {0b00000110, 0b00011001, 0b01011001, 0b10100110, 0b11100000, 0b10100000, 0b01000000, 0b00000000},
    {0b01000110, 0b00011001, 0b01011001, 0b00000110, 0b01010000, 0b11100000, 0b01010000, 0b00000000},
    {0b00000000, 0b00011000, 0b00011000, 0b01111110, 0b01111110, 0b00011000, 0b00011000, 0b00000000},
    {0b00000000, 0b00101000, 0b00010000, 0b00101000, 0b00011110, 0b00010100, 0b00001000, 0b00000000},
    {0b00000000, 0b00000110, 0b00000110, 0b00011110, 0b00011110, 0b01111110, 0b01111110, 0b00000000},
    {0b00000000, 0b00000100, 0b00000110, 0b00111000, 0b01101000, 0b01001000, 0b00110000, 0b00000000},
    {0b00000000, 0b00100100, 0b00011000, 0b00101000, 0b01010100, 0b01100000, 0b00000000, 0b00000000},
    {0b00000010, 0b00001100, 0b00010100, 0b00011000, 0b00110000, 0b01100000, 0b11000000, 0b00000000},
    {0b00110010, 0b00001100, 0b00010100, 0b00101010, 0b01010010, 0b10100000, 0b11000000, 0b00000000},
    {0b00001100, 0b00011000, 0b00110000, 0b01100100, 0b11111100, 0b00100100, 0b00101100, 0b01111000},
    {0b00000000, 0b00000100, 0b00011000, 0b10011000, 0b10100000, 0b01000000, 0b10110000, 0b00000000},
    {0b00000000, 0b01110000, 0b10011100, 0b01010100, 0b01010100, 0b10011100, 0b01110000, 0b00000000},
    {0b01000000, 0b01110000, 0b10111100, 0b01010100, 0b01010100, 0b10111100, 0b01110000, 0b01000000},
    {0b01100000, 0b11110000, 0b11111100, 0b01111100, 0b01111100, 0b11111100, 0b11110000, 0b01100000},
    {0b00000000, 0b00000010, 0b00000100, 0b00011100, 0b10110100, 0b01100000, 0b01000000, 0b00000000},
    {0b00000000, 0b00001000, 0b00010000, 0b00100000, 0b01111000, 0b00110000, 0b01100000, 0b00000000},
    {0b00000000, 0b00000100, 0b00101000, 0b01110000, 0b00111000, 0b00010000, 0b00000000, 0b00000000},
    {0b00000000, 0b00001110, 0b00011010, 0b00101010, 0b01001010, 0b01101010, 0b00111100, 0b00000000},
};
