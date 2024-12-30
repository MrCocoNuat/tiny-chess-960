#pragma once

// many bitfield structs will be specified here but never actually used in code, instead just their backing ints will
// as conversions to ints are not automatic and there is no space to provide them otherwise

enum AdventurerStatusEffect : uint8_t {
    HASTE = 0x1,
    CRIPPLE = 0x2,
    INVISIBLE = 0x3,
    ARCANE_ARMOR = 0x4,
    RECHARGE = 0x5,
    STUN = 0x6,
    BLEED = 0x7
};

struct AdventurerStatusByte {
    AdventurerStatusEffect statusEffect : 3;
    uint8_t duration : 5;
};

struct LevelAndStrength {
    uint8_t level : 4;
    uint8_t excessStrength : 4;
};

struct Adventurer {
    uint8_t health;
    uint16_t location; // figure out an embedding later
    uint8_t satiety;
    uint8_t levelAndStrengh;
    uint8_t experiencePoints;
    uint8_t statusBytes[2];
    uint8_t inventory[10];
};

