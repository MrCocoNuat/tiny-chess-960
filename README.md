# tiny-chess
A full chess playing handheld in 8KiB based on ATtiny84.

For operating instructions, see [guide.md](guide.md)


thinking document:
board state is:
64 bytes for grid, each:
reserved bit
reserved bit
never touched bit
player bit
3b for piece type:
  000 empty (easy to check)
  001 pawn
  010 knight
  011 king
  100 bishop (slider)
  101 rook (slider)
  110 queen (slider)
  111 unused
(sure, piece-major operation is smaller but less extensible)

further ephemeral tiles:
movable mark
cursor

one move is:

byte 0
  before square 6b
  capturing move? indicates read for the capture table
byte 1
  after square 6b
  promotion result 2b
now eeprom can store almost 256 turns, 128 each for 2 players
  less overhead bytes

