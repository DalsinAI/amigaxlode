# XLode for AmigaOS 3.x

XLode is Hisham Muhammad's Lode Runner-like game (2001, GPL 2), from
https://github.com/hishamhm/xlode. This is it on AmigaOS 3.x, 68040 + FPU,
built on acsdl12 (SDL 1.2, SDL_image and SDL_mixer on ACGame and AHI).

## Build

    make -f amiga/Makefile.amigaos \
        STOVE=/path/to/openamigagcc-16.2.0-open4-x86_64-linux \
        NDK=/path/to/gfopensdk/OpenNDK/Include_H \
        ACSDL=/path/to/amicradle-guest/gameports/acsdl12

acsdl12 is built first with its own `Makefile.amigaos`. This gives
`build/amigaos/XLode/` (and `XLode.info` beside it): the program `xlode`,
its pictures, sounds and stages, this ReadMe and the licence, with icons.

## Playing

Run `xlode` from its drawer, from Workbench or a Shell; it finds its files
through PROGDIR:. 320x200 in 256 colours, AGA or RTG (`ENV:ACGame/Display`).

On the title, Up and Down choose, Space or Return starts (or quits).
Cursor keys (or I J K L, or C H T N) run and climb; Right Shift, U or G
digs left; Help, O or R digs right (the Amiga has no End key, so Help is
End). Esc gives up the life, S ends the game. There are four stages.

## Changes for the Amiga

All under `#ifdef __amigaos__`, beside the upstream code:

- The game is built at 320x200 (`-DSIZE_320_200`, upstream's own 320/ set)
  rather than 640x480 in 16 bits: acsdl12 has 8-bit screens only.
- acsdl12 has no `SDL_LoadBMP`, and the BMPs are 24 and 32-bit. The 320/
  pictures are 8-bit PNGs with one shared palette in `amiga/gfx`, made by
  `amiga/convert_gfx.py` (Pillow; run by hand, its output kept here):
  colour 0 black, 255 the magenta colour key, the title and background
  dithered. `GFX_Load` reads `NAME.png` for `NAME.bmp` with `IMG_Load`;
  the screen and the two buffers get the same palette, so blits are copies.
- The 320/ digits are up to 16 rows high: the score is cleared 16 rows
  high, not 8, or old digits showed through below.
- Sound is 22050 Hz mono in the machine's byte order (it asked for 44100
  Hz stereo little-endian). The music is FastTracker XM, which acsdl12
  does not play (4-channel MODs only): the game is silent but for its
  sound effects.
- `amiga/progdir.c`: PROGDIR: is the current directory while it runs,
  and SDL_Quit runs at exit (upstream never calls it).
- `amiga/stack.c`: a 128 KB stack.
- Help is mapped to End; one sprite pointer is set before use.

Byte order: the only files XLode reads itself are the text stages, a byte
at a time. The WAVs are read by acsdl12, which swaps them; the pictures
are converted on the build machine.

State (4 November 2026): builds and links; not yet run in a lab or on a
real Amiga.
