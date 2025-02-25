#pragma once

void initializeBoard(uint8_t theBoard[8][8], uint16_t seed);
void blitBoard();
uint8_t kindOfLegalMove(uint8_t turn, uint8_t theBoard[8][8], uint8_t fromFile, uint8_t fromRank, uint8_t toFile, uint8_t toRank);
bool anyLegalMoves(uint8_t turn, uint8_t theBoard[8][8]);
bool inCheck(uint8_t turn, uint8_t theBoard[8][8]);
// returns the captured piece, if any
uint8_t makeMove(uint8_t turn, uint8_t theBoard[8][8], uint8_t fromFile, uint8_t fromRank, uint8_t toFile, uint8_t toRank);
void makeCastle(uint8_t turn, uint8_t theBoard[8][8], uint8_t kingFile, uint8_t rookFile, uint8_t rank, uint8_t legalCastleKind);
void promote(uint8_t theBoard[8][8], uint8_t file, uint8_t rank, uint8_t promotion);

extern const uint8_t MASK_PIECE_EXISTS;
extern const uint8_t MASK_BLACK_ALLEGIANCE;
extern const uint8_t BIT_BLACK_ALLEGIANCE;
extern const uint8_t MASK_TURN_BLACK;
extern const uint8_t MASK_JUST_DOUBLE_MOVED;  
extern const uint8_t MASK_NEVER_MOVED;
