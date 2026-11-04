#!/usr/bin/env python3
"""XLode's 320x200 pictures as 8-bit PNGs with one shared palette.

    python3 amiga/convert_gfx.py [SRC [DST]]      (default 320 amiga/gfx)

acsdl12 has 8-bit surfaces only and no SDL_LoadBMP, and the 320/*.bmp
files are 24 and 32-bit. This quantises them all to one 256-colour
palette, so every blit in the game is a plain copy:

  index 0    black (the game clears and fills with 0)
  index 255  magenta, ff00ff, the game's colour key
  1..254     the pictures' own colours, median cut over all of them

Sprites and other small pictures are mapped without dithering; the
title and background are dithered. The alpha of the 32-bit BMPs is 0
everywhere and is not used. Needs Pillow; run by hand, the PNGs are
kept in the repository. GPL 2, like XLode.
"""
import sys
from pathlib import Path
from PIL import Image

KEY = (255, 0, 255)
SKIP = {"status.bmp"}          # not loaded by the game


def main():
    src = Path(sys.argv[1] if len(sys.argv) > 1 else "320")
    dst = Path(sys.argv[2] if len(sys.argv) > 2 else "amiga/gfx")
    dst.mkdir(parents=True, exist_ok=True)
    pics = {}
    for f in sorted(src.glob("*.bmp")):
        if f.name not in SKIP:
            pics[f.stem] = Image.open(f).convert("RGB")

    # One mosaic of every picture's colours, small pictures weighted up so
    # the sprites' few colours are kept.
    samples = []
    for im in pics.values():
        px = [p for p in im.getdata() if p != KEY and p != (0, 0, 0)]
        weight = 1 if im.size[0] * im.size[1] > 4096 else 16
        samples.extend(px * weight)
    side = int(len(samples) ** 0.5) + 1
    mosaic = Image.new("RGB", (side, side), (0, 0, 0))
    mosaic.putdata(samples + [(0, 0, 0)] * (side * side - len(samples)))
    q = mosaic.quantize(colors=254, method=Image.Quantize.MEDIANCUT)
    cols = q.getpalette()[:254 * 3]
    cols += [0, 0, 0] * (254 - len(cols) // 3)
    palette = [0, 0, 0] + cols + [0, 0, 0]       # 255 is black while mapping

    ref = Image.new("P", (1, 1))
    ref.putpalette(palette)
    final = palette[:255 * 3] + list(KEY)

    for name, im in pics.items():
        w, h = im.size
        keyed = [p == KEY for p in im.getdata()]
        flat = Image.new("RGB", im.size)
        flat.putdata([(0, 0, 0) if k else p for p, k in zip(im.getdata(), keyed)])
        dither = Image.Dither.FLOYDSTEINBERG if w * h > 4096 else Image.Dither.NONE
        p = flat.quantize(palette=ref, dither=dither)
        idx = list(p.getdata())
        # 255 is the key alone: a colour mapped there (black) goes to 0.
        idx = [255 if k else (0 if i == 255 else i) for i, k in zip(idx, keyed)]
        out = Image.new("P", im.size)
        out.putpalette(final)
        out.putdata(idx)
        out.save(dst / f"{name}.png", optimize=False, bits=8)
    print(f"{len(pics)} pictures to {dst}")


if __name__ == "__main__":
    main()
