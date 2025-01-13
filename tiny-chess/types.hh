#pragma once

// many bitfield structs will be specified here but never actually used in code, instead just their backing ints will
// as conversions to ints are not automatic and there is no space to provide them otherwise

enum Tile : uint8_t {
    EMPTY,
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

enum Castle : uint8_t {
    CASTLE_NONE,
    CASTLE_KINGSIDE,
    CASTLE_QUEENSIDE,
};

// being mandatory, there is no "NONE"
enum Promotion : uint8_t {
    PROMOTE_QUEEN,
    PROMOTE_ROOK,
    PROMOTE_BISHOP,
    PROMOTE_KNIGHT,
};
