// to save sprites, some sprites are doubled up as items and representative tokens on the map

enum SPRITE{
    WALL_1 = 0,
    WALL_2 = 1,
    DOOR = 2,
    CHEST = 3,
    TERRAIN_? = 4,
    GRASS_1 = 5,
    GRASS_2 = 6,
    MELEE_WEAPON = 7, // represents MELEE_WEAPON on map
    MISSILE_WEAPON = 8, // represents RANGED_WEAPON on map
    POTION = 9, // represents POTION on map
    SCROLL = 10, // represents SCROLL on map
    WAND = 11, // represents WAND on map
    ARMOR = 12, // represents ARMOR on map
    AMULET = 13,
    DOWN_STAIR = 14,
    UP_STAIR = 15,

    ADVENTURER = 16,
    RAT = 17,
    SNAKE = 18,
    CRAB = 19,
    GNOLL = 20,
    GOO = 21,
    SKELETON = 22,
    NECRO = 23,
    BAT = 24,
    GOLEM = 25,
    RIPPER = 26,
    SCORPIO = 27,
    EYE = 28,
    YOG = 29,
    FIST = 30,
    MIMIC = 31,

    GOO_CHARGE = 32,
    EYE_CHARGE = 33,
    PEBBLES_1 = 34,
    PEBBLSE_2 = 35,
    THROWN_4 = 36,
    THROWN_3 = 37,
    THROWN_2 = 38,
    THROWN_1 = 39,
    SCROLL_UPGRADE = 10,
    SCROLL_? = 40
    SCROLL_RAGE = 41,
    SCROLL_TERROR = 42,
    SCROLL_RECHARGING = 43,
    POTION_STRENGTH = 9,
    POTION_? = 44,
    POTION_HEALTH = 45,
    POTION_INVISIBIITY = 46,
    POTION_HASTE = 47,

    HEALTH = 48,
    EXPERIENCE = 49,
    FLOOR = 50,
    SATIETY = 51,
    WORN_SHORTSWORD = 52,
    BROADSWORD = 7,
    MELEE_? = 53
    LONGSWORD = 54,
    GREATAXE = 55,
    SHIRT = 12,
    ARMOR_? = 56
    LEATHER = 57,
    MAIL = 58,
    PLATE = 59,
    WAND_MAGIC_MISSILE = 11,
    WAND_? = 60
    KNIFE = 8,
    MISSILE_? = 61,
    HAMMER = 62,
    MEAT_PIE = 63,
    DRUMSTICK = 51
}