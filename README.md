# tiny-chess
A full chess960 (Fischer Random) playing handheld in 8KiB based on ATtiny84.

![device](/github-assets/device.jpg)

Includes a full library for common handheld microcomputer tasks (accepting matrix or joystick input, full support for a SH1106 driver OLED display, speaker), see `board.hh` and `sh1106.hh`.

## Supports
- Local multiplayer with the full ruleset of FIDE chess, minus:
  - draw by threefold repetition
  - draw by 50/75 moves
  - draw by offer
- Extended Chess960 rules - start with the pieces in a symmetric, random arrangement. This freshens up the game and breaks players out of memorized lines, encouraging those with the ability to calculate current board states to rise to the top!
- Set seed play (including the FIDE standard army with seed `518`) or random seed play
- Game saves
- Singleplayer against a tiny chess engine (hey, I've only got 1kB to work with) *in progress*

![main menu](/github-assets/main-menu.jpg)