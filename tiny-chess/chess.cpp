#include "math.hh"
#include "chess.hh"
#include <Arduino.h>
#include "types.hh"

const uint8_t MASK_TURN_BLACK = 1;

const uint8_t MASK_JUST_DOUBLE_MOVED = 1 << 5;  // for en-passant
const uint8_t MASK_NEVER_MOVED = 1 << 4;        // for castling
const uint8_t BIT_BLACK_ALLEGIANCE = 3;
const uint8_t MASK_BLACK_ALLEGIANCE = 1 << 3;
const uint8_t MASK_PIECE_EXISTS = 0x7;

// store for check calculations
uint8_t kingFiles[2];
uint8_t kingRanks[2];

// assumes input is in [0,960)
void initializeBoard(uint8_t theBoard[8][8], uint16_t seed) {

  // wipe the board
  uint8_t *p = &theBoard[0][0];
  for (uint8_t i = 0; i < 8 * 8; i++) {
    *p++ = EMPTY;
  }

  // start with standard position for now
  theBoard[0][0] = ROOK;
  theBoard[1][0] = KNIGHT;
  theBoard[2][0] = BISHOP;
  theBoard[3][0] = QUEEN;
  theBoard[4][0] = KING;
  theBoard[5][0] = BISHOP;
  theBoard[6][0] = KNIGHT;
  theBoard[7][0] = ROOK;
  theBoard[0][1] = PAWN;
  theBoard[1][1] = PAWN;
  theBoard[2][1] = PAWN;
  theBoard[3][1] = PAWN;
  theBoard[4][1] = PAWN;
  theBoard[5][1] = PAWN;
  theBoard[6][1] = PAWN;
  theBoard[7][1] = PAWN;
  kingFiles[0] = 4;
  kingRanks[0] = 0;

  theBoard[0][7] = ROOK | MASK_BLACK_ALLEGIANCE;
  theBoard[1][7] = KNIGHT | MASK_BLACK_ALLEGIANCE;
  theBoard[2][7] = BISHOP | MASK_BLACK_ALLEGIANCE;
  theBoard[3][7] = QUEEN | MASK_BLACK_ALLEGIANCE;
  theBoard[4][7] = KING | MASK_BLACK_ALLEGIANCE;
  theBoard[5][7] = BISHOP | MASK_BLACK_ALLEGIANCE;
  theBoard[6][7] = KNIGHT | MASK_BLACK_ALLEGIANCE;
  theBoard[7][7] = ROOK | MASK_BLACK_ALLEGIANCE;
  theBoard[0][6] = PAWN | MASK_BLACK_ALLEGIANCE;
  theBoard[1][6] = PAWN | MASK_BLACK_ALLEGIANCE;
  theBoard[2][6] = PAWN | MASK_BLACK_ALLEGIANCE;
  theBoard[3][6] = PAWN | MASK_BLACK_ALLEGIANCE;
  theBoard[4][6] = PAWN | MASK_BLACK_ALLEGIANCE;
  theBoard[5][6] = PAWN | MASK_BLACK_ALLEGIANCE;
  theBoard[6][6] = PAWN | MASK_BLACK_ALLEGIANCE;
  theBoard[7][6] = PAWN | MASK_BLACK_ALLEGIANCE;
  kingFiles[1] = 4;
  kingRanks[1] = 7;
}

void fillPiece(uint8_t *square, PieceType pieceType) {
}


bool bishopAttacks(uint8_t theBoard[8][8], uint8_t fromFile, uint8_t fromRank, uint8_t toFile, uint8_t toRank) {
  if (
    fromFile == toFile || (toFile - fromFile != toRank - fromRank && toFile - fromFile != fromRank - toRank)) {
    return false;
  }

  int8_t fileDirection = (toFile > fromFile) ? 1 : -1;
  int8_t rankDirection = (toRank > fromRank) ? 1 : -1;

  bool unblockedDiagonal = true;
  for (uint8_t f = fromFile + fileDirection, r = fromRank + rankDirection; f != toFile; f += fileDirection, r += rankDirection)
    unblockedDiagonal &= !(theBoard[f][r] & MASK_PIECE_EXISTS);
  return unblockedDiagonal;
}

bool rookAttacks(uint8_t theBoard[8][8], uint8_t fromFile, uint8_t fromRank, uint8_t toFile, uint8_t toRank) {
  if ((toFile != fromFile && toRank != fromRank) || (toFile == fromFile && toRank == fromRank)) {
    return false;
  }

  int8_t fileDirection = (toFile > fromFile) ? 1 : (toFile == fromFile) ? 0
                                                                        : -1;
  int8_t rankDirection = (toRank > fromRank) ? 1 : (toRank == fromRank) ? 0
                                                                        : -1;

  bool unblockedStraight = true;
  for (uint8_t f = fromFile + fileDirection, r = fromRank + rankDirection; f != toFile || r != toRank; f += fileDirection, r += rankDirection)
    unblockedStraight &= !(theBoard[f][r] & MASK_PIECE_EXISTS);
  return unblockedStraight;
}

// simplest check for "can piece on XX attack square YY", disregarding the allegiance of pieces, any king threats that would open, etc.
// FIXME: for some reason the switch table is just not working right,
// the same check for == PIECETYPE outside does not produce the same results as a case PIECETYPE: inside
// this costs around 400B... suspiciously terrible! likely the compiled code is optimizing out the pieceType computation and doesn't even do the same job
// also inefficient for finding legal slider moves, this calls "is there an empty path to here" on the same path over and over again
bool attacks(uint8_t theBoard[8][8], uint8_t fromFile, uint8_t fromRank, uint8_t toFile, uint8_t toRank) {

  uint8_t pieceType = theBoard[fromFile][fromRank] & MASK_PIECE_EXISTS;
  if (pieceType == PAWN) {
    // for the pawn to go "forward", how does its rank change? 1 for white, -1 for black
    int8_t forwardOne = 1 - 2 * ((theBoard[fromFile][fromRank] & MASK_BLACK_ALLEGIANCE) >> BIT_BLACK_ALLEGIANCE);
    return toRank - fromRank == forwardOne && (fromFile - toFile == 1 || toFile - fromFile == 1);
  }
  if (pieceType == KNIGHT) {
    return (norm(toRank - fromRank) == 2 && norm(toFile - fromFile) == 1) || (norm(toRank - fromRank) == 1 && norm(toFile - fromFile) == 2);
  }
  if (pieceType == KING) {
    return (toRank - fromRank || toFile - fromFile) && (norm(toRank - fromRank) <= 1) && (norm(toFile - fromFile) <= 1);
  }
  if (pieceType == QUEEN) {
    return bishopAttacks(theBoard, fromFile, fromRank, toFile, toRank) || rookAttacks(theBoard, fromFile, fromRank, toFile, toRank);
  }
  if (pieceType == BISHOP) {
    return bishopAttacks(theBoard, fromFile, fromRank, toFile, toRank);
  }
  if (pieceType == ROOK) {
    return rookAttacks(theBoard, fromFile, fromRank, toFile, toRank);
  }
  return false;
}

bool moves(uint8_t theBoard[8][8], uint8_t fromFile, uint8_t fromRank, uint8_t toFile, uint8_t toRank) {

  uint8_t pieceType = theBoard[fromFile][fromRank] & MASK_PIECE_EXISTS;
  if (pieceType == PAWN) {
    if (fromFile != toFile)
      return false;
    // for the pawn to go "forward", how does its rank change? 1 for white, -1 for black
    int8_t forwardOne = 1 - 2 * ((theBoard[fromFile][fromRank] & MASK_BLACK_ALLEGIANCE) >> BIT_BLACK_ALLEGIANCE);
    uint8_t startingRank = (theBoard[fromFile][fromRank] & MASK_BLACK_ALLEGIANCE) ? 6 : 1;
    if (fromRank == startingRank) {
      // double move? check the intervening square
      if (!(theBoard[fromFile][fromRank + forwardOne] & MASK_PIECE_EXISTS) && toRank - fromRank == forwardOne * 2)
        return true;
    }
    return toRank - fromRank == forwardOne;
  }
  // all other pieces move like they attack
  return attacks(theBoard, fromFile, fromRank, toFile, toRank);
}

// pre-supposes that from actually has a piece on it
bool isLegal(uint8_t turn, uint8_t theBoard[8][8], uint8_t fromFile, uint8_t fromRank, uint8_t toFile, uint8_t toRank) {
  // needs to be a move or capture in the first place
  if (!(
        (moves(theBoard, fromFile, fromRank, toFile, toRank) && !(theBoard[toFile][toRank] & MASK_PIECE_EXISTS))
        //FIXME: pawns can attack to an unoccupied square en passant
        || (attacks(theBoard, fromFile, fromRank, toFile, toRank) && theBoard[toFile][toRank] & MASK_PIECE_EXISTS && ((theBoard[fromFile][fromRank] & MASK_BLACK_ALLEGIANCE) != (theBoard[toFile][toRank] & MASK_BLACK_ALLEGIANCE))))) {
    return false;
  }

  // is the king the one moving?
  bool kingMove = (theBoard[fromFile][fromRank] & MASK_PIECE_EXISTS) == KING;

  // legal move is one that does not have your own king under attack immediately afterwards
  // simulate the move
  uint8_t destinationContents = theBoard[toFile][toRank];
  theBoard[toFile][toRank] = theBoard[fromFile][fromRank];
  theBoard[fromFile][fromRank] = EMPTY;
  // check for opening own king to attack
  uint8_t kingFile = kingMove ? toFile : kingFiles[turn & MASK_TURN_BLACK];
  uint8_t kingRank = kingMove ? toRank : kingRanks[turn & MASK_TURN_BLACK];

  bool opensKing = false;
  for (uint8_t f = 0; !opensKing && f < 8; f++) {
    for (uint8_t r = 0; r < 8; r++) {
      // just check for opponent's pieces:
      if (!(theBoard[f][r] & MASK_PIECE_EXISTS) || ((theBoard[f][r] & MASK_BLACK_ALLEGIANCE) == (theBoard[kingFile][kingRank] & MASK_BLACK_ALLEGIANCE))) {
        continue;
      }
      // yeah this is a lot of calls to attacks... but who cares? it finishes fast enough, and space (both memory and program code) is much much more valuable than time here:
      if (attacks(theBoard, f, r, kingFile, kingRank)) {
        opensKing = true;
        break;
      }
    }
  }
  // put the piece back
  theBoard[fromFile][fromRank] = theBoard[toFile][toRank];
  theBoard[toFile][toRank] = destinationContents;

  if (opensKing == true)
    return false;
  return true;
}

bool inCheck(uint8_t turn, uint8_t theBoard[8][8]) {
  uint8_t kingFile = kingFiles[turn & MASK_TURN_BLACK];
  uint8_t kingRank = kingRanks[turn & MASK_TURN_BLACK];
  for (uint8_t f = 0; f < 8; f++) {
    for (uint8_t r = 0; r < 8; r++) {
      // only consider opponent piece
      if (!(theBoard[f][r] & MASK_PIECE_EXISTS && ((theBoard[kingFile][kingRank] & MASK_BLACK_ALLEGIANCE) != (theBoard[f][r] & MASK_BLACK_ALLEGIANCE))))
        ;
      if (attacks(theBoard, f, r, kingFile, kingRank))
        return true;
    }
  }
  return false;
}

bool anyLegalMoves(uint8_t turn, uint8_t theBoard[8][8]) {
  // wahaahahahahahaha quad loops let's go!
  // not that bad realistically, either there are many legal moves and this returns fast,
  // or there are no legal moves, but very few pieces to iterate over
  for (uint8_t fromF = 0; fromF < 8; fromF++) {
    for (uint8_t fromR = 0; fromR < 8; fromR++) {
      // only consider own piece
      if (!(theBoard[fromF][fromR] & MASK_PIECE_EXISTS) || (((theBoard[fromF][fromR] & MASK_BLACK_ALLEGIANCE) >> BIT_BLACK_ALLEGIANCE) != (turn & MASK_TURN_BLACK)))
        continue;
      for (uint8_t toF = 0; toF < 8; toF++) {
        for (uint8_t toR = 0; toR < 8; toR++) {
          if (isLegal(turn, theBoard, fromF, fromR, toF, toR)) {
            return true;
          }
        }
      }
    }
  }
  return false;
}
