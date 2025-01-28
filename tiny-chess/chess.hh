#pragma once

void initializeBoard(uint8_t theBoard[8][8], uint16_t seed);
void blitBoard();
bool isLegal(uint8_t turn, uint8_t theBoard[8][8], uint8_t fromFile, uint8_t fromRank, uint8_t toFile, uint8_t toRank);
bool anyLegalMoves(uint8_t turn, uint8_t theBoard[8][8]);
bool inCheck(uint8_t turn, uint8_t theBoard[8][8]);
// returns the captured piece, if any
uint8_t makeMove(uint8_t turn, uint8_t theBoard[8][8], uint8_t fromFile, uint8_t fromRank, uint8_t toFile, uint8_t toRank);

extern const uint8_t MASK_PIECE_EXISTS;
extern const uint8_t MASK_BLACK_ALLEGIANCE;
extern const uint8_t BIT_BLACK_ALLEGIANCE;
extern const uint8_t MASK_TURN_BLACK;
extern const uint8_t MASK_JUST_DOUBLE_MOVED;  
extern const uint8_t MASK_NEVER_MOVED;

// FIXME extremely cursed. fix by abstracting the whole board
extern uint8_t kingFiles[2];
extern uint8_t kingRanks[2];