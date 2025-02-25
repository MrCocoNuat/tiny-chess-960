#include "sh1106.hh"
#include "board.hh"
#include "sh1106.hh"
#include "chess.hh"
#include "types.hh"

uint8_t board[8][8];   // file-major
uint8_t turn;          // even = white odd = black

uint8_t cursorFile;
uint8_t cursorRank;
uint8_t handFile;
uint8_t handRank;
Promotion promotionChoice = PROMOTE_QUEEN;   // queen first by default

// prevent the same input from immediately repeating, like xrate
uint8_t bannedJoystickState;
uint8_t previousJoystickState;
uint8_t bannedRemainingTime;

#define JOYSTICK_DELAY 96  // between first input and second input
#define JOYSTICK_RATE 32   // between second and third, and ...

#define cursorAnimationConstant 0x6
#define promotionAnimationConstant 0x30
// different values create fun effects when blitted really really fast onto the oled
// 0x8 is a 2Hz breathing
// 0x4 is 4Hz
// 0x6 is a dim cursor frame, cool
// 0x10 is a 0.3Hz blink of the cursor, further slower acts like you would expect


uint8_t spriteAt(uint8_t file, uint8_t rank) {
  return board[file][rank] & MASK_PIECE_EXISTS  // is there a piece at all?
           ? board[file][rank] & 0xF            // render the piece itself with its allegiance
           : ((file + rank) & 0x1) << 3;        // empty, render the tile instead: black tile (0 << 3) for even squares, white (1 << 3) for odd
}

void blitCursor() {
  moveTo(cursorFile << 3, cursorRank << 3);
  plotSprite(
    // animation depends on timer 0:
    !(TCNT0 & cursorAnimationConstant)
      ? 0xF  // cursor sprite
      : spriteAt(cursorFile, cursorRank));
}

void blitBoard() {
  for (int file = 0; file < 8; file++) {
    for (int rank = 0; rank < 8; rank++) {
      moveTo(file << 3, rank << 3);
      plotSprite(spriteAt(file, rank));
    }
  }
}

void blitLegalMoves(){
  for (uint8_t f = 0; f < 8; f++) {
    for (uint8_t r = 0; r < 8; r++) {
      if (kindOfLegalMove(turn, board, handFile, handRank, f, r)) {
        moveTo(f << 3, r << 3);
        plotSprite(7);
      }
    }
  }
}

void blitPromotion(uint8_t file, uint8_t rank, Promotion piece){
    moveTo(file << 3, rank << 3);
    plotSprite(
      // animation depends on timer 0:
      (TCNT0 & promotionAnimationConstant)
        ? (piece | (board[file][rank] & MASK_BLACK_ALLEGIANCE))  // promotion target
        : 0x0
    );
}

enum SuperState {
  MAIN_MENU,
  NEW_TURN,
  CONTEMPLATING,
  PIECE_IN_HAND,
  PROMOTING,
  VIEWING_HISTORY,
  GAME_OVER
};
SuperState superState = CONTEMPLATING;

void initialize(uint16_t seed){
  initializeBoard(board, seed); //temp: allow resuming the game
}


void doIt() {

  blitBoard();

  for (;;) {
    // handle animations
    if (superState == PROMOTING){
      // cursor is on a promoting pawn!
      blitPromotion(cursorFile, cursorRank, promotionChoice);
    } else {
      // blit cursor according to timer
      blitCursor();
    }
    if (bannedRemainingTime)
      bannedRemainingTime--;

    //fixme: need handleAnimations which runs before input banning
    //fixme: some states don't wait for input

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
        if (!anyLegalMoves(turn, board)) {
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
          // does it have any moves?
          bool canMove = false;
          for(uint8_t f = 0; !canMove && f < 8; f++){
            for(uint8_t r = 0; !canMove && r < 8; r++){
              if (kindOfLegalMove(turn, board, cursorFile, cursorRank, f, r)){
                canMove = true;
              }
            }
          }
          if (!canMove) break; // no!

          superState = PIECE_IN_HAND;
          handFile = cursorFile;
          handRank = cursorRank;
          blitLegalMoves(); // FIXME optimize to return num legal moves duh
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

          blitLegalMoves();
          break;
        }

        // change mind?
        if (joystickState & (1 << STICK_RL)) {
          cursorFile = handFile;
          cursorRank = handRank;
          superState = CONTEMPLATING;
          blitBoard();
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
          uint8_t legalMove = kindOfLegalMove(turn, board, handFile, handRank, cursorFile, cursorRank);
          if (legalMove) {
            if (legalMove == CASTLE_KINGSIDE || legalMove == CASTLE_QUEENSIDE){
              makeCastle(turn, board, handFile, cursorFile, handRank, legalMove);
            } else {
              makeMove(turn, board, handFile, handRank, cursorFile, cursorRank);
            }
            if (legalMove == PROMOTION){
              superState = PROMOTING;

            } else {
              superState = NEW_TURN;
            }
            blitBoard();
            break;
          }
          // not legal move? ignore
          break;
        }

        break;
      case PROMOTING:
        // display promotion choices, left stick scrolls
        if (joystickState & ((1 << STICK_LL) | (1 << STICK_LR) | (1 << STICK_LU) | (1 << STICK_LD))){
          promotionChoice = promotionChoice == PROMOTE_KNIGHT? PROMOTE_QUEEN : promotionChoice - 1;
        }
        // right stick confirms
        if (joystickState & ((1 << STICK_RL) | (1 << STICK_RR) | (1 << STICK_RU) | (1 << STICK_RD))){
          promote(board, cursorFile, cursorRank, promotionChoice);
          blitBoard();
          promotionChoice = PROMOTE_QUEEN;
          superState = NEW_TURN;
        }
        break;
      case VIEWING_HISTORY:
        break;
      case GAME_OVER:
        blitBoard();
        if (inCheck(turn, board)) {
          plotChar('C', 70, 0);
        } else {
          plotChar('S', 70, 0);
        }
        return;  // TEMP: obviously don't just return from this then main...
    }
  }
}

//RESUME: kindOfLegalCastle is not showing castling as legal