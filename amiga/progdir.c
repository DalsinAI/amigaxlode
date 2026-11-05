/* XLode opens its pictures, sounds and stages by their bare names, from
 * the current directory. Run from a Shell anywhere, or from Workbench,
 * it finds them in its own drawer: PROGDIR: is made the current
 * directory, and the one it was is given back at exit.
 * XLode never calls SDL_Quit (LIB_Done is never called), so the screen
 * and AHI would stay open after it: SDL_Quit runs at exit too, before
 * the directory is given back.
 * The AmiCradle Team. GPL 2, like XLode. */
#include <stdlib.h>
#include <proto/dos.h>
#include <SDL.h>

static BPTR progdir_lock;
static BPTR old_dir;

static void XLode_amigaDone(void)
{
    if (progdir_lock) {
        CurrentDir(old_dir);
        UnLock(progdir_lock);
        progdir_lock = 0;
    }
}

void XLode_amigaInit(void)
{
    progdir_lock = Lock((CONST_STRPTR)"PROGDIR:", ACCESS_READ);
    if (progdir_lock) {
        old_dir = CurrentDir(progdir_lock);
        atexit(XLode_amigaDone);
    }
    atexit(SDL_Quit);
}
