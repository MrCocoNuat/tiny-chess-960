#include "persistence.hh"
#include "sh1106.hh"
#include <avr/pgmspace.h>
#include "board.hh"
#include "math.hh"

#define SEED_MAX 960
#define SEED_STANDARD_FIDE_ARMY 518

const uint16_t FLAG_NEW_GAME_SEED = 0x0000; //TODO: this is not playing nicely when combined with game seed. So use it only for continues

// menu choices:
// 0 continue
// 1 new game - standard
// 2 random
// 3 set seed

uint8_t currentMenuChoice;
uint8_t cursorPage[] = {5, 3, 2, 1};
uint8_t cursorX = 12;
int8_t currentEditingDigit = -1; // 0,1,2. for digit places. anything else is not editing this
uint8_t currentNewGameSeed[] = {0,0,0};

// prevent the same input from immediately repeating, like xrate
uint8_t bannedJoystickState2;
uint8_t previousJoystickState2;
uint8_t bannedRemainingTime2;

uint16_t randomByte;

#define JOYSTICK_DELAY2 48  // between first input and second input
#define JOYSTICK_RATE2 24   // between second and third, and ...

// return:
// FLAG_NEW_GAME_SEED ? <new game seed> : <continue game>
uint16_t doMenu(){
    moveTo(6, 7 << 3);
    plotText(PSTR("== TINY CHESS960 =="));
    moveTo(24, 5 << 3);
    plotText(PSTR("CONTINUE NOTYET"));
    moveTo(24, 4 << 3);
    plotText(PSTR("NEW GAME:"));
    moveTo(36, 3 << 3);
    plotText(PSTR("STANDARD"));
    moveTo(36, 2 << 3);
    plotText(PSTR("RANDOM SEED"));
    moveTo(36, 1 << 3);
    plotText(PSTR("SET SEED:"));

    for(;;){
      plotChar('>', cursorX, cursorPage[currentMenuChoice]);
      plotChar('0' + currentNewGameSeed[0], 90, 1);
      plotChar('0' + currentNewGameSeed[1], 96, 1);
      plotChar('0' + currentNewGameSeed[2], 102, 1);
      if (currentEditingDigit >= 0){
        plotChar('^', 90 + currentEditingDigit * 6, 0);
      }
      randomByte = nextByte();

      if (bannedRemainingTime2)
        bannedRemainingTime2--;

      uint8_t joystickState = getJoystickState();
      // demand exactly 1 button is pressed!
      if ((!joystickState) || (joystickState & (joystickState - 1))) {
        previousJoystickState2 = 0;
        bannedRemainingTime2 = 0;
        continue;
      }

      // check that this input isn't banned
      if (joystickState == bannedJoystickState2 && bannedRemainingTime2)
        continue;
      // and set the input repeat delays
      bannedJoystickState2 = joystickState;
      bannedRemainingTime2 = JOYSTICK_RATE2;
      if (previousJoystickState2 != bannedJoystickState2) {
        previousJoystickState2 = bannedJoystickState2;
        bannedRemainingTime2 = JOYSTICK_DELAY2;
      }

      if (joystickState & (1 << STICK_RR)){
          switch(currentMenuChoice){
            case 0: // continue
             // return 0; //TODO: persistence not yet supported
              continue;
            case 1: // standard game
              return FLAG_NEW_GAME_SEED | SEED_STANDARD_FIDE_ARMY;
            case 2: // random game
              return FLAG_NEW_GAME_SEED | ((randomByte << 8 | nextByte()) % SEED_MAX);
            case 3: // set seed game
              uint16_t seed = currentNewGameSeed[0] * 100 + currentNewGameSeed[1] * 10 + currentNewGameSeed[2];
              return FLAG_NEW_GAME_SEED | seed;
          }
      }

      if (currentEditingDigit >= 0){
          if (joystickState & (1 << STICK_LU)){
              currentNewGameSeed[currentEditingDigit] = currentNewGameSeed[currentEditingDigit] == 9 ? 9 : currentNewGameSeed[currentEditingDigit] + 1;
              if (currentNewGameSeed[0] == 9 && currentNewGameSeed[1] > 5){
                  currentNewGameSeed[1] = 5;
                  currentNewGameSeed[2] = 9;
              }
          }
          if (joystickState & (1 << STICK_LD)){
              currentNewGameSeed[currentEditingDigit] = currentNewGameSeed[currentEditingDigit] == 0 ? 0 : currentNewGameSeed[currentEditingDigit] - 1;
          }
      } else {
          if (joystickState & (1 << STICK_LU)){
              plotChar(' ', cursorX, cursorPage[currentMenuChoice]);
              currentMenuChoice = currentMenuChoice == 0? 0 : currentMenuChoice - 1;
          }
          if (joystickState & (1 << STICK_LD)){
              plotChar(' ', cursorX, cursorPage[currentMenuChoice]);
              currentMenuChoice = currentMenuChoice == 3? 3 : currentMenuChoice + 1;
          }
      }
      if (currentMenuChoice == 3){
          if (joystickState & (1 << STICK_LL)){
              plotChar(' ', 90 + currentEditingDigit * 6, 0);
              currentEditingDigit = currentEditingDigit == -1 ? -1 : currentEditingDigit - 1;
          }
          if (joystickState & (1 << STICK_LR)){
              plotChar(' ', 90 + currentEditingDigit * 6, 0);
              currentEditingDigit = currentEditingDigit == 2 ? 2 : currentEditingDigit + 1;
          }
      }


    }
}