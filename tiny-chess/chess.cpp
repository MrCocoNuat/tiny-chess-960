#include "sh1106.hh"
#include "board.hh"
#include "math.hh"

uint8_t board[8][8];   // file-major here
uint8_t board2[8][8];  // same, used as a buffer in legal moves calculation
uint8_t turn;          // even = white odd = black
const uint8_t MASK_TURN_BLACK = 1;

const uint8_t MASK_PIECE_EXISTS = 0x7;
const uint8_t MASK_JUST_DOUBLE_MOVED = 1 << 5;  // for en-passant
const uint8_t MASK_NEVER_MOVED = 1 << 4;        //
#define BIT_BLACK_ALLEGIANCE 3
const uint8_t MASK_BLACK_ALLEGIANCE = 1 << 3;

uint8_t cursorFile;
uint8_t cursorRank;
uint8_t handFile;
uint8_t handRank;

// store for check calculations
uint8_t kingFiles[2];
uint8_t kingRanks[2];

enum SuperState {
  MAIN_MENU,
  NEW_TURN,
  CONTEMPLATING,
  PIECE_IN_HAND,
  VIEWING_HISTORY,
  GAME_OVER
};
SuperState superState = CONTEMPLATING;

enum PieceType {
  EMPTY = 0,
  // walkers
  PAWN = 0b01,
  KNIGHT = 0b10,
  KING = 0b11,
  // sliders
  BISHOP = 0b100,
  ROOK = 0b101,
  QUEEN = 0b110
};

// assumes input is in [0,960)
void fillInOpeningPositions(uint16_t seed) {

  // wipe the board
  uint8_t *p = &board[0][0];
  for (uint8_t i = 0; i < 8 * 8; i++) {
    *p++ = EMPTY;
  }

  // start with standard position for now
  board[0][0] = ROOK;
  board[1][0] = KNIGHT;
  board[2][0] = BISHOP;
  board[3][0] = QUEEN;
  board[4][0] = KING;
  board[5][0] = BISHOP;
  board[6][0] = KNIGHT;
  board[7][0] = ROOK;
  board[0][1] = PAWN;
  board[1][1] = PAWN;
  board[2][1] = PAWN;
  board[3][1] = PAWN;
  board[4][1] = PAWN;
  board[5][1] = PAWN;
  board[6][1] = PAWN;
  board[7][1] = PAWN;
  kingFiles[0] = 4;
  kingRanks[0] = 0;

  board[0][7] = ROOK | MASK_BLACK_ALLEGIANCE;
  board[1][7] = KNIGHT | MASK_BLACK_ALLEGIANCE;
  board[2][7] = BISHOP | MASK_BLACK_ALLEGIANCE;
  board[3][7] = QUEEN | MASK_BLACK_ALLEGIANCE;
  board[4][7] = KING | MASK_BLACK_ALLEGIANCE;
  board[5][7] = BISHOP | MASK_BLACK_ALLEGIANCE;
  board[6][7] = KNIGHT | MASK_BLACK_ALLEGIANCE;
  board[7][7] = ROOK | MASK_BLACK_ALLEGIANCE;
  board[0][6] = PAWN | MASK_BLACK_ALLEGIANCE;
  board[1][6] = PAWN | MASK_BLACK_ALLEGIANCE;
  board[2][6] = PAWN | MASK_BLACK_ALLEGIANCE;
  board[3][6] = PAWN | MASK_BLACK_ALLEGIANCE;
  board[4][6] = PAWN | MASK_BLACK_ALLEGIANCE;
  board[5][6] = PAWN | MASK_BLACK_ALLEGIANCE;
  board[6][6] = PAWN | MASK_BLACK_ALLEGIANCE;
  board[7][6] = PAWN | MASK_BLACK_ALLEGIANCE;
  kingFiles[1] = 4;
  kingRanks[1] = 7;
}

uint8_t spriteAt(uint8_t file, uint8_t rank) {
  return board[file][rank] & MASK_PIECE_EXISTS  // is there a piece at all?
           ? board[file][rank] & 0xF            // render the piece itself with its allegiance
           : ((file + rank) & 0x1) << 3;        // empty, render the tile instead: black tile (0 << 3) for even squares, white (1 << 3) for odd
}

void blitBoard() {
  for (int file = 0; file < 8; file++) {
    for (int rank = 0; rank < 8; rank++) {
      moveTo(file << 3, rank << 3);
      plotSprite(spriteAt(file, rank));
    }
  }
}


void fillPiece(uint8_t *square, PieceType pieceType) {
}

#define cursorConstant 0x6
// different values create fun effects when blitted really really fast onto the oled
// 0x8 is a 2Hz breathing
// 0x4 is 4Hz
// 0x6 is a dim cursor frame, cool
// 0x1 is a 0.3Hz blink of the cursor, further slower acts like you would expect

void blitCursor() {
  moveTo(cursorFile << 3, cursorRank << 3);
  plotSprite(
    // animation depends on timer 0:
    !(TCNT0 & cursorConstant)
      ? 0xF  // cursor sprite
      : spriteAt(cursorFile, cursorRank));
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
      // double move
      if (toRank - fromRank == forwardOne * 2)
        return true;
    }
    return toRank - fromRank == forwardOne;
  }
  // all other pieces move like they attack
  return attacks(theBoard, fromFile, fromRank, toFile, toRank);
}

// pre-supposes that from actually has a piece on it
bool isLegal(uint8_t fromFile, uint8_t fromRank, uint8_t toFile, uint8_t toRank) {
  // needs to be a move or capture in the first place
  if (!(
        (moves(board, fromFile, fromRank, toFile, toRank) && !(board[toFile][toRank] & MASK_PIECE_EXISTS))
        //FIXME: pawns can attack to an unoccupied square en passant
        || (attacks(board, fromFile, fromRank, toFile, toRank) && board[toFile][toRank] & MASK_PIECE_EXISTS && ((board[fromFile][fromRank] & MASK_BLACK_ALLEGIANCE) != (board[toFile][toRank] & MASK_BLACK_ALLEGIANCE))))) {
    return false;
  }

  // is the king the one moving?
  bool kingMove = (board[fromFile][fromRank] & MASK_PIECE_EXISTS) == KING;

  // legal move is one that does not have your own king under attack immediately afterwards
  // simulate the move
  uint8_t destinationContents = board[toFile][toRank];
  board[toFile][toRank] = board[fromFile][fromRank];
  board[fromFile][fromRank] = EMPTY;
  // check for opening own king to attack
  uint8_t kingFile = kingMove ? toFile : kingFiles[turn & MASK_TURN_BLACK];
  uint8_t kingRank = kingMove ? toRank : kingRanks[turn & MASK_TURN_BLACK];

  bool opensKing = false;
  for (uint8_t f = 0; !opensKing && f < 8; f++) {
    for (uint8_t r = 0; r < 8; r++) {
      // just check for opponent's pieces:
      if (!(board[f][r] & MASK_PIECE_EXISTS) || ((board[f][r] & MASK_BLACK_ALLEGIANCE) == (board[kingFile][kingRank] & MASK_BLACK_ALLEGIANCE))) {
        continue;
      }
      // yeah this is a lot of calls to attacks... but who cares? it finishes fast enough, and space (both memory and program code) is much much more valuable than time here:
      if (attacks(board, f, r, kingFile, kingRank)) {
        opensKing = true;
        break;
      }
    }
  }
  // put the piece back
  board[fromFile][fromRank] = board[toFile][toRank];
  board[toFile][toRank] = destinationContents;

  if (opensKing == true)
    return false;
  return true;
}

bool inCheck() {
  uint8_t kingFile = kingFiles[turn & MASK_TURN_BLACK];
  uint8_t kingRank = kingRanks[turn & MASK_TURN_BLACK];
  for (uint8_t f = 0; f < 8; f++) {
    for (uint8_t r = 0; r < 8; r++) {
      // only consider opponent piece
      if (!(board[f][r] & MASK_PIECE_EXISTS && ((board[kingFile][kingRank] & MASK_BLACK_ALLEGIANCE) != (board[f][r] & MASK_BLACK_ALLEGIANCE))))
        ;
      if (attacks(board, f, r, kingFile, kingRank))
        return true;
    }
  }
  return false;
}

bool anyLegalMoves() {
  // wahaahahahahahaha quad loops let's go!
  // not that bad realistically, either there are many legal moves and this returns fast,
  // or there are no legal moves, but very few pieces to iterate over
  for (uint8_t fromF = 0; fromF < 8; fromF++) {
    for (uint8_t fromR = 0; fromR < 8; fromR++) {
      // only consider own piece
      if (!(board[fromF][fromR] & MASK_PIECE_EXISTS) || (((board[fromF][fromR] & MASK_BLACK_ALLEGIANCE) >> BIT_BLACK_ALLEGIANCE) != (turn & MASK_TURN_BLACK)))
        continue;
      for (uint8_t toF = 0; toF < 8; toF++) {
        for (uint8_t toR = 0; toR < 8; toR++) {
          if (isLegal(fromF, fromR, toF, toR)) {
            return true;
          }
        }
      }
    }
  }
  return false;
}

// prevent the same input from immediately repeating, like xrate
uint8_t bannedJoystickState;
uint8_t previousJoystickState;
uint8_t bannedRemainingTime;

#define JOYSTICK_DELAY 96  // between first input and second input
#define JOYSTICK_RATE 32   // between second and third, and ...

void doIt() {
  for (;;) {
    // handle animations
    // blit cursor according to timer
    blitCursor();
    if (bannedRemainingTime)
      bannedRemainingTime--;

    //fixme: need handleAnimations which runs before input banning
    if (superState == PIECE_IN_HAND) {
      //temp: debug
      for (uint8_t f = 0; f < 8; f++) {
        for (uint8_t r = 0; r < 8; r++) {
          if (isLegal(handFile, handRank, f, r)) {
            moveTo(f << 3, r << 3);
            plotSprite(7);
          }
        }
      }
    }

    uint8_t joystickState = getJoystickState();
    // demand exactly 1 button is pressed!
    if ((!joystickState) || (joystickState & (joystickState - 1))) {
      previousJoystickState = 0;
      bannedRemainingTime = 0;
      continue;
    }

    // check that this input isn't banned
    if (joystickState == bannedJoystickState && bannedRemainingTime)
      continue;
    // and set the input repeat delays
    bannedJoystickState = joystickState;
    bannedRemainingTime = JOYSTICK_RATE;
    if (previousJoystickState != bannedJoystickState) {
      previousJoystickState = bannedJoystickState;
      bannedRemainingTime = JOYSTICK_DELAY;
    }

    switch (superState) {
      case NEW_TURN:
        turn++;
        // mate?
        if (!anyLegalMoves()) {
          superState = GAME_OVER;
          break;
        }
        superState = CONTEMPLATING;
        break;

      case CONTEMPLATING:

        // movement?
        if (joystickState & STICK_ALL_LEFT) {

          // cursor movement triggered: restore the sprite on which the cursor was
          moveTo(cursorFile << 3, cursorRank << 3);
          plotSprite(spriteAt(cursorFile, cursorRank));

          // which button/bit of joystickState was that?
          // no branches
          cursorFile += (joystickState == (1 << STICK_LR)) - (joystickState == (1 << STICK_LL));
          cursorRank += (joystickState == (1 << STICK_LU)) - (joystickState == (1 << STICK_LD));

          // loop around modulo 8 - smart avoidance of branches!
          cursorFile &= 0b111;
          cursorRank &= 0b111;

          break;
        }

        // piece up?
        if (joystickState & (1 << STICK_RU)) {
          // not own piece (piece is black == turn counter is black)?
          if (!(board[cursorFile][cursorRank] & MASK_PIECE_EXISTS) || (((board[cursorFile][cursorRank] & MASK_BLACK_ALLEGIANCE) >> BIT_BLACK_ALLEGIANCE) != (turn & MASK_TURN_BLACK)))
            break;
          superState = PIECE_IN_HAND;
          handFile = cursorFile;
          handRank = cursorRank;
        }

        // history?
        // transition to history

        break;
      case MAIN_MENU:
      case PIECE_IN_HAND:
        // move cursor? TODO copypasted
        if (joystickState & STICK_ALL_LEFT) {

          // cursor movement triggered: restore the sprite on which the cursor was
          moveTo(cursorFile << 3, cursorRank << 3);
          plotSprite(spriteAt(cursorFile, cursorRank));

          // which button/bit of joystickState was that?
          // no branches
          cursorFile += (joystickState == (1 << STICK_LR)) - (joystickState == (1 << STICK_LL));
          cursorRank += (joystickState == (1 << STICK_LU)) - (joystickState == (1 << STICK_LD));

          // loop around modulo 8 - smart avoidance of branches!
          cursorFile &= 0b111;
          cursorRank &= 0b111;

          break;
        }

        // piece down?
        if (joystickState & (1 << STICK_RD)) {

          // put piece back down where it was?
          if (cursorFile == handFile && cursorRank == handRank) {
            superState = CONTEMPLATING;
            blitBoard();
            break;
          }
          // put piece down anew to make a move;
          if (isLegal(handFile, handRank, cursorFile, cursorRank)) {
            bool kingMove = (board[handFile][handRank] & MASK_PIECE_EXISTS) == KING;
            if (kingMove) {
              kingFiles[turn & MASK_TURN_BLACK] = cursorFile;
              kingRanks[turn & MASK_TURN_BLACK] = cursorRank;
            }

            board[cursorFile][cursorRank] = board[handFile][handRank];  // FIXME need to clear special flags! in a movePiece function
            board[handFile][handRank] = EMPTY;

            superState = NEW_TURN;
            blitBoard();
            break;
          }
          // not legal move? ignore
          break;
        }

        break;
      case VIEWING_HISTORY:
        break;
      case GAME_OVER:
        blitBoard();
        if (inCheck()) {
          plotChar('C', 70, 0);
        } else {
          plotChar('S', 70, 0);
        }
        return;  // TEMP: obviously don't just return from this then main...
    }
  }
}