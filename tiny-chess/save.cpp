#include "persistence.hh"
#include "chess.hh"
#include "types.hh"

/*
  Overall structure: 512B are split into
  1B reserved
  1B for how many turns have passed - this is how far to read the saved data
  2B for the saved seed
  2B*254 for saved moves
 */

/*
  Each move is recorded in 2 bytes
  and is not stored reversibly (that is, it is impossible to "go back" just 1 turn, to do so requires replaying the whole history)
  because of the need to keep track of flags like NEVER_MOVED, JUST_DOUBLE_MOVED, etc. for captured pieces

  This is not so bad since applying known-legal moves to a board state is extremely fast compared to checking for legal moves

  2B in bitfield:
  LSB 0 1 2 3 4 5 6 7 8 9 A B C D E F MSB
  0x0 fromF fromR -toF- -toR- pr/cs

  fromFile: 0x00-0x02 (needs only store 1 out of 8 integers)
  fromRank: 0x03-0x05
  toFile: 0x06-0x08
  toRank: 0x09-0x0B
  promotion/castle: 0x0C-0x0E (needs to store 1 out of 8 values, 4 promotions, 2 castlings, and 1 more for none-of-the-above)
  reserved: 0x0F
 */

// exceed this and die
#define MAX_TURN 253

bool saveTurnCount(uint8_t turn){
  put(0, (uint16_t) turn);
}
uint8_t getTurnCount(){
  return (uint8_t) get(0);
}

void saveSeed(uint16_t seed){
  put(1, seed);
}
uint16_t getSeed(){
  return get(1);
}

void saveMove(uint8_t turn, uint8_t fromFile, uint8_t fromRank, uint8_t toFile, uint8_t toRank, uint8_t specialMove){
    saveTurnCount(turn + 1);

    // Packing the move data into a 16-bit value
    uint16_t move = 0;

    move |= (fromFile << 0);      // Store fromFile in bits 0-2
    move |= (fromRank << 3);      // Store fromRank in bits 3-5
    move |= (toFile << 6);        // Store toFile in bits 6-8
    move |= (toRank << 9);        // Store toRank in bits 9-11
    move |= (specialMove << 12);    // Store promotion or castle in bits 12-14

    put(turn + 2, move);
}
void applySavedMove(uint8_t theBoard[8][8], uint8_t turn){
    uint16_t packedMove = get(turn + 2);

    uint8_t specialMove = (packedMove >> 12) & 0x07;
    uint8_t toRank = (packedMove >> 9) & 0x07;
    uint8_t toFile = (packedMove >> 6) & 0x07;
    uint8_t fromRank = (packedMove >> 3) & 0x07;
    uint8_t fromFile = packedMove & 0x07;

    if (specialMove == CASTLE_QUEENSIDE || specialMove == CASTLE_KINGSIDE){
        makeCastle(turn, theBoard, fromFile, toFile, fromRank, specialMove);
    } else {
        makeMove(turn, theBoard, fromFile, fromRank, toFile, toRank);
        if (specialMove != NO_SPECIAL_MOVE){ // must be a promotion!
            promote(theBoard, toFile, toRank, specialMove);
        }
    }
}

void applySavedMovesUpTo(uint8_t theBoard[8][8], uint8_t turn){
    for (uint8_t t = 0; t < turn; t++){
        applySavedMove(theBoard, t);
    }
}