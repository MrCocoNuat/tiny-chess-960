#pragma once

// many bitfield structs will be specified here but never actually used in code, instead just their backing ints will
// as conversions to ints are not automatic and there is no space to provide them otherwise

enum PieceType {
    EMPTY = 0,
    // walkers
    PAWN = 0b001,
    KING = 0b010,
    KNIGHT = 0b011,
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

enum LegalCastle : uint8_t {
    CASTLE_KINGSIDE = 0x01,
    CASTLE_QUEENSIDE = 0x02,
};

const uint8_t NO_SPECIAL_MOVE = 0x07;

enum LegalPromotion : uint8_t {
    PROMOTION_ELIGIBLE = 0xFF,
};

// being mandatory, there is no "NONE"
enum Promotion : uint8_t {
    PROMOTE_QUEEN = QUEEN,
    PROMOTE_ROOK = ROOK,
    PROMOTE_BISHOP = BISHOP,
    PROMOTE_KNIGHT = KNIGHT,
    // these are consecutive values!
};

