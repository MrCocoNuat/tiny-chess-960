#import <avr/common.h>

struct InMemoryFloor {
    uint32_t seed; // what was the SEED used to generate this floor? it is responsible for:
    // Permanent terrain
    uint8_t room[6][2][2]; // Coordinates of corners of up to 6 rooms, 2 corners a room, then x and y
    uint8_t corridor[6][3][2]; // Coordinates of door, corner, door for up to 6 corridors, 3 points a corridor, then x and y. Can't do 2-corner optimization since these are L-shaped, not rectangular


};

struct InMemoryFloor floor;