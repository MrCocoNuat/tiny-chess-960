#include "sh1106.hh"

uint8_t board[8][8]; // file-major here
uint8_t turn; // even = white odd = black

uint8_t NEVER_MOVED = 0x10;
uint8_t BLACK_ALLEGIANCE = 0x8;

uint8_t cursor; // stays where last player put it

enum EngineState{
    MAIN_MENU,
    CONTEMPLATING,
    PIECE_IN_HAND,
    VIEWING_HISTORY,
    GAME_OVER
};

enum PieceType{
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
void fillInOpeningPositions(uint16_t seed){

    // wipe
    uint8_t* p = &board[0][0];
    for (uint8_t i = 0; i < 8*8; i++){
        *p++ = 0;
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

    board[0][7] = ROOK | BLACK_ALLEGIANCE;
    board[1][7] = KNIGHT | BLACK_ALLEGIANCE;
    board[2][7] = BISHOP | BLACK_ALLEGIANCE;
    board[3][7] = QUEEN | BLACK_ALLEGIANCE;
    board[4][7] = KING | BLACK_ALLEGIANCE;
    board[5][7] = BISHOP | BLACK_ALLEGIANCE;
    board[6][7] = KNIGHT | BLACK_ALLEGIANCE;
    board[7][7] = ROOK | BLACK_ALLEGIANCE;
    board[0][6] = PAWN | BLACK_ALLEGIANCE;
    board[1][6] = PAWN | BLACK_ALLEGIANCE;
    board[2][6] = PAWN | BLACK_ALLEGIANCE;
    board[3][6] = PAWN | BLACK_ALLEGIANCE;
    board[4][6] = PAWN | BLACK_ALLEGIANCE;
    board[5][6] = PAWN | BLACK_ALLEGIANCE;
    board[6][6] = PAWN | BLACK_ALLEGIANCE;
    board[7][6] = PAWN | BLACK_ALLEGIANCE;
}

void blitBoard(){
    for(int file = 0; file < 8; file++){
        for(int rank = 0; rank < 8; rank++){
            moveTo(file << 3, rank << 3);
            plotSprite(
              board[file][rank] & 0x7 // is there a piece at all?
              ? board[file][rank] & 0xF // render the piece itself with its allegiance
              : ((file + rank) & 0x1) << 3  // empty, render the tile instead: black tile (0 << 3) for even squares, white (1 << 3) for odd 
            );
        }
    }
}

void fillPiece(uint8_t *square, PieceType pieceType){

}

void doIt(){
    // wait for inputs:
    // move cursor?
      // do it
    // piece up?
      // if own piece: transition to piece up state
    // history?
    // transition to history
}

uint8_t legalTos[21];
void legalMovesFrom(uint8_t from){
    // at most 21 legal moves from any one piece
    uint8_t fromF = from >> 5, fromR = (from >> 2) & 0b111;
    switch(board[fromF][fromR]){

    }
}

void move(uint8_t from, uint8_t to){
    // there's no validation of legality here
    uint8_t fromF = from >> 5, fromR = (from >> 2) & 0b111;
    uint8_t toF = to >> 5, toR = (to >> 2) & 0b111;

    // move the piece, leaving an empty
    board[toF][toR] = board[fromF][fromR];
    board[fromF][fromR] = EMPTY;
}