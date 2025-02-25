#include "math.hh"
#include "chess.hh"
#include <Arduino.h>
#include "sh1106.hh"
#include "types.hh"

const uint8_t MASK_TURN_BLACK = 1;

// bits 6 and 7 reserved for now
const uint8_t MASK_JUST_DOUBLE_MOVED = 1 << 5;  // for en-passant
const uint8_t MASK_NEVER_MOVED = 1 << 4;        // for castling
const uint8_t BIT_BLACK_ALLEGIANCE = 3;
const uint8_t MASK_BLACK_ALLEGIANCE = 1 << 3;
const uint8_t MASK_PIECE_EXISTS = 0x7;
// when just passing a turn, these are preserved;
const uint8_t MASK_LONG_LIVED = MASK_PIECE_EXISTS | MASK_BLACK_ALLEGIANCE | MASK_NEVER_MOVED;
// when moving a piece, these are always preserved
const uint8_t MASK_PERMANENT = MASK_PIECE_EXISTS | MASK_BLACK_ALLEGIANCE;

// for castling:
// although the names don't really match, the king and queen can be anywhere relative to each other
const uint8_t CASTLE_KINGSIDE_KING_FILE = 6;
const uint8_t CASTLE_KINGSIDE_ROOK_FILE = 5;
const uint8_t CASTLE_QUEENSIDE_KING_FILE = 2;
const uint8_t CASTLE_QUEENSIDE_ROOK_FILE = 3;

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

  uint16_t seedRemnant = seed;
  // derive the initial board state from the seed:
  // bishops start on opposite colors
  uint8_t bishopFile1 = ((seedRemnant % 4) << 1) + 1;
  seedRemnant /= 4;
  uint8_t bishopFile2 = (seedRemnant % 4) << 1;
  seedRemnant /= 4;

  theBoard[bishopFile1][0] = BISHOP | MASK_NEVER_MOVED;
  theBoard[bishopFile1][7] = BISHOP | MASK_BLACK_ALLEGIANCE | MASK_NEVER_MOVED;
  theBoard[bishopFile2][0] = BISHOP | MASK_NEVER_MOVED;
  theBoard[bishopFile2][7] = BISHOP | MASK_BLACK_ALLEGIANCE | MASK_NEVER_MOVED;

  // queen is somewhere in the 6 remaining files
  uint8_t queenPosition = seedRemnant % 6;
  seedRemnant /= 6;
  for(uint8_t queenFile = 0; queenFile < 8;  queenFile++){
    if (theBoard[queenFile][0] == EMPTY){
      if (queenPosition == 0){
        theBoard[queenFile][0] = QUEEN | MASK_NEVER_MOVED;
        theBoard[queenFile][7] = QUEEN | MASK_BLACK_ALLEGIANCE | MASK_NEVER_MOVED;
      }
      queenPosition--;
    }
  }

  // place knights in the 5 remaining files
  uint8_t knightPosition1 = 0, knightPosition2 = 1;
  for(; seedRemnant > 0; seedRemnant--){
    knightPosition2++;
    if (knightPosition2 > 4){
      knightPosition1++;
      knightPosition2 = knightPosition1 + 1;
    }
  }

  for(uint8_t knightFile = 0, knightPosition = 0; knightFile < 8; knightFile++){
    if (theBoard[knightFile][0] == EMPTY){
      if (knightPosition == knightPosition1 || knightPosition == knightPosition2){
        theBoard[knightFile][0] = KNIGHT | MASK_NEVER_MOVED;
        theBoard[knightFile][7] = KNIGHT | MASK_BLACK_ALLEGIANCE | MASK_NEVER_MOVED;
      }
      knightPosition++;
    }
  }

  // king goes between the rooks
  for(uint8_t file = 0, remainingPosition = 0; file < 8; file++){
    if (theBoard[file][0] == EMPTY){
      if (remainingPosition == 1){
        theBoard[file][0] = KING | MASK_NEVER_MOVED; //TODO: sometimes castling disappears the king? check right-side in 000
        theBoard[file][7] = KING | MASK_BLACK_ALLEGIANCE| MASK_NEVER_MOVED;
        kingFiles[0] = file;
        kingFiles[1] = file;
        kingRanks[0] = 0;
        kingRanks[1] = 7;
      } else {
        theBoard[file][0] = ROOK | MASK_NEVER_MOVED;
        theBoard[file][7] = ROOK | MASK_BLACK_ALLEGIANCE | MASK_NEVER_MOVED;
      }
      remainingPosition++;
    }
  }

  // then all the pawns
  theBoard[0][1] = PAWN | MASK_NEVER_MOVED;
  theBoard[1][1] = PAWN | MASK_NEVER_MOVED;
  theBoard[2][1] = PAWN | MASK_NEVER_MOVED;
  theBoard[3][1] = PAWN | MASK_NEVER_MOVED;
  theBoard[4][1] = PAWN | MASK_NEVER_MOVED;
  theBoard[5][1] = PAWN | MASK_NEVER_MOVED;
  theBoard[6][1] = PAWN | MASK_NEVER_MOVED;
  theBoard[7][1] = PAWN | MASK_NEVER_MOVED;

  theBoard[0][6] = PAWN | MASK_BLACK_ALLEGIANCE | MASK_NEVER_MOVED;
  theBoard[1][6] = PAWN | MASK_BLACK_ALLEGIANCE | MASK_NEVER_MOVED;
  theBoard[2][6] = PAWN | MASK_BLACK_ALLEGIANCE | MASK_NEVER_MOVED;
  theBoard[3][6] = PAWN | MASK_BLACK_ALLEGIANCE | MASK_NEVER_MOVED;
  theBoard[4][6] = PAWN | MASK_BLACK_ALLEGIANCE | MASK_NEVER_MOVED;
  theBoard[5][6] = PAWN | MASK_BLACK_ALLEGIANCE | MASK_NEVER_MOVED;
  theBoard[6][6] = PAWN | MASK_BLACK_ALLEGIANCE | MASK_NEVER_MOVED;
  theBoard[7][6] = PAWN | MASK_BLACK_ALLEGIANCE | MASK_NEVER_MOVED;
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


/*
const uint8_t CASTLE_KINGSIDE_KING_FILE = 6;
const uint8_t CASTLE_KINGSIDE_ROOK_FILE = 5;
const uint8_t CASTLE_QUEENSIDE_KING_FILE = 2;
const uint8_t CASTLE_QUEENSIDE_ROOK_FILE = 3;
 */

// pre-supposes both pieces have never moved, and are actually a king and rook - check before calling this expensive function!
uint8_t kindOfLegalCastle(uint8_t turn, uint8_t theBoard[8][8], uint8_t kingFile, uint8_t rookFile, uint8_t rank) {

  bool isCastlingKingside = rookFile > kingFile;
  uint8_t kingToFile = isCastlingKingside ? CASTLE_KINGSIDE_KING_FILE : CASTLE_QUEENSIDE_KING_FILE;
  uint8_t rookToFile = isCastlingKingside ? CASTLE_KINGSIDE_ROOK_FILE : CASTLE_QUEENSIDE_ROOK_FILE;

  // there is no requirement that the king and rook move in opposite directions,
  // or even in any particular directions... so these must be calculated
  int8_t kingDirection = (kingToFile > kingFile) ? 1 : -1;

  // king's path isn't clear: any square being attacked is no good!
  for (uint8_t kingPassingFile = kingFile; kingPassingFile != kingToFile + kingDirection; kingPassingFile += kingDirection) {
    // intervening? must be empty square

    if (kingPassingFile != kingFile && kingPassingFile != rookFile && theBoard[kingPassingFile][rank] & MASK_PIECE_EXISTS)
      return false;
    // and a not attacked square
    // no need to lift up the king or rook for this, if either is pinned by an attack it is coming from the home rank anyway, so...
    // the king is either already in check (king... pin?) or will be (rook pin)
    for (uint8_t f = 0; f < 8; f++) {
      for (uint8_t r = 0; r < 8; r++) {
        // only consider opponent piece
        if (!(theBoard[f][r] & MASK_PIECE_EXISTS && ((theBoard[kingFile][rank] & MASK_BLACK_ALLEGIANCE) != (theBoard[f][r] & MASK_BLACK_ALLEGIANCE))))
          continue;
        if (attacks(theBoard, f, r, kingPassingFile, rank))
          return false;
      }
    }
  }

  // rook's path isn't clear
  int8_t rookDirection = (rookToFile > rookFile) ? 1 : -1;
  for (uint8_t rookPassingFile = rookFile; rookPassingFile != rookToFile + rookDirection; rookPassingFile += rookDirection) {
    // intervening? must be empty square
    if (rookPassingFile != rookFile && rookPassingFile != kingFile && theBoard[rookPassingFile][rank] & MASK_PIECE_EXISTS)
      return false;
  }


  return isCastlingKingside? CASTLE_KINGSIDE : CASTLE_QUEENSIDE;
}

// pre-supposes that from actually has a piece on it
uint8_t kindOfLegalMove(uint8_t turn, uint8_t theBoard[8][8], uint8_t fromFile, uint8_t fromRank, uint8_t toFile, uint8_t toRank) {
  // needs to be a move or capture in the first place
  // kindOfLegalMove is called a lot and moves/attacks calls are real expensive, short circuit when possible
  bool isMove = !(theBoard[toFile][toRank] & MASK_PIECE_EXISTS)
                && moves(theBoard, fromFile, fromRank, toFile, toRank);
  bool isAttack = theBoard[toFile][toRank] & MASK_PIECE_EXISTS
                  && ((theBoard[fromFile][fromRank] & MASK_BLACK_ALLEGIANCE) != (theBoard[toFile][toRank] & MASK_BLACK_ALLEGIANCE))
                  && attacks(theBoard, fromFile, fromRank, toFile, toRank);
  bool isEnPassant = (theBoard[fromFile][fromRank] & MASK_PIECE_EXISTS) == PAWN
                     && theBoard[toFile][fromRank] & MASK_JUST_DOUBLE_MOVED
                     && ((theBoard[fromFile][fromRank] & MASK_BLACK_ALLEGIANCE) != (theBoard[toFile][fromRank] & MASK_BLACK_ALLEGIANCE))
                     && attacks(theBoard, fromFile, fromRank, toFile, toRank);
  bool isPromotion = (theBoard[fromFile][fromRank] & MASK_PIECE_EXISTS) == PAWN
                     && ((toRank == 7 && !(theBoard[fromFile][fromRank] & MASK_BLACK_ALLEGIANCE))
                         || (toRank == 0 && theBoard[fromFile][fromRank] & MASK_BLACK_ALLEGIANCE));
  bool isPotentialCastle = (fromRank == toRank)
                  && (theBoard[fromFile][fromRank] & MASK_PIECE_EXISTS) == KING
                  && (theBoard[toFile][toRank] & MASK_PIECE_EXISTS) == ROOK
                  && theBoard[fromFile][fromRank] & MASK_NEVER_MOVED
                  && theBoard[toFile][toRank] & MASK_NEVER_MOVED
                  && ((theBoard[fromFile][fromRank] & MASK_BLACK_ALLEGIANCE) == (theBoard[toFile][fromRank] & MASK_BLACK_ALLEGIANCE));
  // castling has to handle its own check logic, it is too different from the rest, so it can return authoritative true
  if (isPotentialCastle) return kindOfLegalCastle(turn, theBoard, fromFile, toFile, fromRank);

  if (!isMove && !isAttack && !isEnPassant) return false;

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

  if (opensKing)
    return false;
  if (isPromotion){
    return PROMOTION;
  }
  return true;
}

bool inCheck(uint8_t turn, uint8_t theBoard[8][8]) {
  uint8_t kingFile = kingFiles[turn & MASK_TURN_BLACK];
  uint8_t kingRank = kingRanks[turn & MASK_TURN_BLACK];
  for (uint8_t f = 0; f < 8; f++) {
    for (uint8_t r = 0; r < 8; r++) {
      // only consider opponent piece
      if (!(theBoard[f][r] & MASK_PIECE_EXISTS && ((theBoard[kingFile][kingRank] & MASK_BLACK_ALLEGIANCE) != (theBoard[f][r] & MASK_BLACK_ALLEGIANCE))))
        continue;
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
          if (kindOfLegalMove(turn, theBoard, fromF, fromR, toF, toR)) {
            return true;
          }
        }
      }
    }
  }
  return false;
}

uint8_t makeMove(uint8_t turn, uint8_t theBoard[8][8], uint8_t fromFile, uint8_t fromRank, uint8_t toFile, uint8_t toRank) {
  // move the piece,
  bool kingMove = (theBoard[fromFile][fromRank] & MASK_PIECE_EXISTS) == KING;
  if (kingMove) {
    kingFiles[turn & MASK_TURN_BLACK] = toFile;
    kingRanks[turn & MASK_TURN_BLACK] = toRank;
  }
  // is this an en passant capture?
  uint8_t enPassantCapture = ((theBoard[fromFile][fromRank] & MASK_PIECE_EXISTS) == PAWN && theBoard[toFile][fromRank] & MASK_JUST_DOUBLE_MOVED);
  // remember the captured piece,
  uint8_t capturedPiece = enPassantCapture ? theBoard[toFile][fromRank] : theBoard[toFile][toRank];
  // capture, clearing any special flags,
  theBoard[toFile][toRank] = theBoard[fromFile][fromRank] & MASK_PERMANENT;
  // and clear the origin square - IF it is not the same as the destination, as could be in the case of castling in chess960
  if (toFile != fromFile || toRank != fromRank){
    theBoard[fromFile][fromRank] = EMPTY;
  }
  // damn en passant
  theBoard[toFile][fromRank] &= enPassantCapture ? 0 : 0xFF;

  // clear short-lived flags from the board on every move // TEMP: break out?
  for (uint8_t f = 0; f < 8; f++) {
    for (uint8_t r = 0; r < 8; r++) {
      theBoard[f][r] &= MASK_LONG_LIVED;
    }
  }
  // but make sure to set any ones that are needed anew

  // pawn double move?
  if ((theBoard[toFile][toRank] & MASK_PIECE_EXISTS) == PAWN && (toRank - fromRank == 2 || fromRank - toRank == 2)) {
    theBoard[toFile][toRank] |= MASK_JUST_DOUBLE_MOVED;
  }
  return capturedPiece;
}

void makeCastle(uint8_t turn, uint8_t theBoard[8][8], uint8_t kingFile, uint8_t rookFile, uint8_t rank, uint8_t legalCastleKind){
  uint8_t kingToFile = legalCastleKind == CASTLE_KINGSIDE ? CASTLE_KINGSIDE_KING_FILE : CASTLE_QUEENSIDE_KING_FILE;
  uint8_t rookToFile = legalCastleKind == CASTLE_KINGSIDE ? CASTLE_KINGSIDE_ROOK_FILE : CASTLE_QUEENSIDE_ROOK_FILE;
  makeMove(turn, theBoard, kingFile, rank, kingToFile, rank);
  makeMove(turn, theBoard, rookFile, rank, rookToFile, rank);
}

void promote(uint8_t theBoard[8][8], uint8_t file, uint8_t rank, uint8_t promotion){
  theBoard[file][rank] = (theBoard[file][rank] & ~MASK_PIECE_EXISTS) | promotion;
}