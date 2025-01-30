#pragma once

// many bitfield structs will be specified here but never actually used in code, instead just their backing ints will
// as conversions to ints are not automatic and there is no space to provide them otherwise

enum PieceType {
    EMPTY = 0,
    // walkers
    PAWN = 0b001,
    KNIGHT = 0b010,
    KING = 0b011,
    // sliders
    BISHOP = 0b100,
    ROOK = 0b101,
    QUEEN = 0b110
};

enum Tile : uint8_t {
    TILE_EMPTY,
    WHITE_KING,
    WHITE_QUEEN,
    WHITE_BISHOP,
    WHITE_KNIGHT,
    WHITE_ROOK,
    WHITE_PAWN,
    BLACK_KING,
    BLACK_QUEEN,
    BLACK_BISHOP,
    BLACK_KNIGHT,
    BLACK_ROOK,
    BLACK_PAWN,
};

// dual purpose, returned by kindOfLegalCastle and saved to eeprom as moves
enum LegalCastle : uint8_t {
    CASTLE_ILLEGAL = 0,
    CASTLE_KINGSIDE = 0xF0, // TEMP: these need to be invalid move ids for eeprom
    CASTLE_QUEENSIDE = 0xF1,
};

// being mandatory, there is no "NONE"
enum Promotion : uint8_t {
    PROMOTE_QUEEN,
    PROMOTE_ROOK,
    PROMOTE_BISHOP,
    PROMOTE_KNIGHT,
};

