"""
Piskel supports an export to C array option (this is spritesheet.c). But this is meant for full ARGB sprites.
I need monochrome values and can't accept multiplying needed storage by 8.

Therefore, this script reads in the source of that array and converts it into a form better suited for the microcontroller:

turn the array static const uint32_t new_piskel_data[4][32*32] which is for 32*32 pixels per "frame" (really just different spritesheets)
into a static const uint8_t new_piskel_data[4*4*4][8] which separates out each actual 8*8 monochrome sprite
"""

import numpy as np
import re

# Constants
NEW_PISKEL_FRAME_WIDTH = 32
NEW_PISKEL_FRAME_HEIGHT = 32
SPRITE_WIDTH = 8
SPRITE_HEIGHT = 8
NUM_FRAMES = 4
SPRITES_PER_FRAME = (NEW_PISKEL_FRAME_WIDTH // SPRITE_WIDTH) * (NEW_PISKEL_FRAME_HEIGHT // SPRITE_HEIGHT)

# Function to read ARGB data from the C source file
def read_argb_data_from_file(filename):
    with open(filename, 'r') as file:
        content = file.read()

    # Use regex to find the pixel data array with any valid identifier name
    matches = re.findall(r'static const uint32_t ([\w]+)\[\d+\]\[\d+\] = \{(.*?)\};', content, re.DOTALL)

    if not matches:
        raise ValueError("No pixel data found in the file.")

    # We only need the pixel data part
    pixel_data_string = matches[0][1]  # Get the second capturing group (the pixel data)
    # Clean up the string and convert it into a list of integers
    pixel_data_list = []
    for num in pixel_data_string.split(','):
        num = num.strip().strip('{} \n')  # Remove braces and whitespace
        if num:
            pixel_data_list.append(int(num, 16))

    # Reshape the list into a single dimension for each frame
    return np.array(pixel_data_list).reshape((NUM_FRAMES, NEW_PISKEL_FRAME_HEIGHT * NEW_PISKEL_FRAME_WIDTH))

# Prepare the output array
sprites = np.zeros((NUM_FRAMES * SPRITES_PER_FRAME, SPRITE_WIDTH), dtype=np.uint8)  # Adjusted width

# Read the ARGB data from the C file
new_piskel_data = read_argb_data_from_file('spritesheet.c')

# Process each frame
for frame_index in range(NUM_FRAMES):
    for sprite_y in range(NEW_PISKEL_FRAME_HEIGHT // SPRITE_HEIGHT):
        for sprite_x in range(NEW_PISKEL_FRAME_WIDTH // SPRITE_WIDTH):
            sprite_index = frame_index * SPRITES_PER_FRAME + sprite_y * (NEW_PISKEL_FRAME_WIDTH // SPRITE_WIDTH) + sprite_x
            for x in range(SPRITE_WIDTH):
                for y in range(SPRITE_HEIGHT):
                    pixel_index = (sprite_y * SPRITE_HEIGHT + y) * NEW_PISKEL_FRAME_WIDTH + (sprite_x * SPRITE_WIDTH + x)
                    argb_pixel = new_piskel_data[frame_index][pixel_index]
                    # Ensure argb_pixel is treated as a single scalar value
                    if argb_pixel == 0xffffffff:
                        sprites[sprite_index][x] |= 1 << (SPRITE_HEIGHT - 1 - y)  # Set to 1 for white (column-wise)
                    elif argb_pixel == 0xff000000:
                        sprites[sprite_index][x] |= 0 << (SPRITE_HEIGHT - 1 - y)  # Set to 0 for black (column-wise)
                    # No action needed for other colors, they will default to 0

# Write the monochrome sprites to a new C++ file
with open('monochrome-spritesheet.c++', 'w') as file:
    file.write("#include <cstdint>\n\n")
    file.write("static const uint8_t monochrome_sprites[{}][{}] = {{\n".format(NUM_FRAMES * SPRITES_PER_FRAME, SPRITE_WIDTH))

    for sprite in sprites:
        file.write("    {")
        file.write(", ".join(f"0b{pixel:08b}" for pixel in sprite))  # Format as binary
        file.write("},\n")

    file.write("};\n")

print("Monochrome sprites written to monochrome-spritesheet.c++")
