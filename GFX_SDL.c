
#include <SDL/SDL.h>

typedef SDL_Surface GFX_Image;

SDL_Surface* framebuffer;

#ifdef __amigaos__
/* acsdl12 has 8-bit surfaces only and no SDL_LoadBMP. The pictures are
   8-bit PNGs sharing one palette (amiga/convert_gfx.py): 0 is black,
   255 is the magenta colour key. The screen and the game's buffers get
   the same palette, so every blit is a plain copy. */
#include <SDL/SDL_image.h>
#include <string.h>

#define GFX_KEY_INDEX 255

static SDL_Color gfx_palette[256];

static void GFX_Fail(const char* what, const char* name) {
   fprintf(stderr, "XLode: %s %s: %s\n", what, name, SDL_GetError());
   exit(20);                  /* SDL_Quit runs at exit (amiga/progdir.c) */
}
#endif

void GFX_Done() {
}

void GFX_Free(GFX_Image* image) {
   SDL_FreeSurface(image);
}

GFX_Image* GFX_Load(char* filename) {
   GFX_Image* image;
#ifdef __amigaos__
   char name[64];
   char* dot;
   strncpy(name, filename, sizeof(name) - 5);
   name[sizeof(name) - 5] = 0;
   dot = strrchr(name, '.');
   strcpy(dot ? dot : name + strlen(name), ".png");
   image = IMG_Load(name);
   if (!image)
      GFX_Fail("cannot load", name);
   SDL_SetColorKey(image, SDL_SRCCOLORKEY, GFX_KEY_INDEX);
#else
   image = SDL_LoadBMP(filename);
   SDL_SetColorKey(image, SDL_SRCCOLORKEY|SDL_RLEACCEL, 0xff00ff);
#endif
   return image;
}

GFX_Image* GFX_CreateBlank(int w, int h) {
   GFX_Image* image;
#ifdef __amigaos__
   image = SDL_CreateRGBSurface(0, w, h, 8, 0, 0, 0, 0);
   if (!image)
      GFX_Fail("cannot make", "a buffer");
   SDL_SetColors(image, gfx_palette, 0, 256);
#else
   image = SDL_CreateRGBSurface(0, w, h, 16, 0, 0, 0, 0);
#endif
   return image;
}

void GFX_Clear(GFX_Image* image) {
   SDL_FillRect(image, NULL, 0x00000000);
}

void GFX_Draw(GFX_Image* dest, GFX_Image* src, int destX, int destY) {
   SDL_Rect rect;
   rect.x = destX;
   rect.y = destY;
   SDL_BlitSurface(src, NULL, dest, &rect);
}

void GFX_Blit(GFX_Image* src, GFX_Image* dest, int srcX, int srcY, int destX, int destY, int srcW, int srcH) {
   SDL_Rect srcRect;
   SDL_Rect destRect;
   srcRect.x = srcX;
   srcRect.y = srcY;
   srcRect.w = srcW;
   srcRect.h = srcH;
   destRect.x = destX;
   destRect.y = destY;
   SDL_BlitSurface(src, &srcRect, dest, &destRect);
}

void GFX_FillRectangle(GFX_Image* dest, int x1, int y1, int x2, int y2, int color) {
   SDL_Rect rect;
   rect.x = x1;
   rect.y = y1;
   rect.w = x2-x1+1;
   rect.h = y2-y1+1;
   SDL_FillRect(dest, &rect, color);
}

int GFX_Init(int resX, int resY, int depth) {
#ifdef __amigaos__
   SDL_Surface* palette_source;
   SDL_Init(SDL_INIT_VIDEO);
   framebuffer = SDL_SetVideoMode(resX, resY, 8, SDL_HWSURFACE|SDL_DOUBLEBUF);
   if (!framebuffer)
      GFX_Fail("cannot open", "the screen");
   SDL_WM_SetCaption("XLode", "XLode");
   SDL_ShowCursor(SDL_DISABLE);
   palette_source = IMG_Load(RES_DIR "background.png");
   if (!palette_source)
      GFX_Fail("cannot load", RES_DIR "background.png");
   memcpy(gfx_palette, palette_source->format->palette->colors, sizeof(gfx_palette));
   SDL_FreeSurface(palette_source);
   SDL_SetColors(framebuffer, gfx_palette, 0, 256);
   SDL_FillRect(framebuffer, NULL, 0);
#else
   SDL_Init(SDL_INIT_VIDEO);
   framebuffer = SDL_SetVideoMode(resX, resY, depth, SDL_HWSURFACE|SDL_DOUBLEBUF);
#endif
   return 1;
}

GFX_Image* GFX_Screen() {
   return SDL_GetVideoSurface();
}

void GFX_DisplayBuffer(GFX_Image* image) {
   SDL_BlitSurface(image, NULL, framebuffer, NULL);
   SDL_Flip(framebuffer);
}

void GFX_UpdateScreen() {
   SDL_Flip(framebuffer);
}

void GFX_ToggleFullScreen() {
   // 2013-10-08 This got me in a video mode that I could not get out of!
   // SDL_WM_ToggleFullScreen(framebuffer);
}
