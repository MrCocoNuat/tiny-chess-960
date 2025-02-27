#include "types.hh"

bool saveTurnCount(uint8_t turn);
uint8_t getTurnCount();
void saveSeed(uint16_t seed);
uint16_t getSeed();
void saveMove(uint8_t turn, uint8_t fromfile, uint8_t fromRank, uint8_t toFile, uint8_t toRank, uint8_t specialMove);
void applySavedMove(uint8_t theBoard[8][8], uint8_t turn); // because returning an object is just annoying, apply it directly
void applySavedMovesUpTo(uint8_t theBoard[8][8], uint8_t turn);