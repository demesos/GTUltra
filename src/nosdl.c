/*
 * nosdl.c - minimal SDL2 replacement for the command line tools (gt2reloc).
 *
 * gt2reloc links the editor's objects, which reference SDL for window,
 * audio, clipboard and timer handling. The relocator itself uses none of it:
 * it calls SDL_Init(0) and tries to open audio, which may fail (it then runs
 * the SID emulation without sound output). These stubs let gt2reloc link
 * without SDL, so it can be built as a fully static binary.
 *
 * Window, renderer and audio functions report failure; timing and memory
 * functions do the real thing.
 */
#include <SDL.h>
#include <stdlib.h>
#include <time.h>

static const char *nosdl_error = "SDL not available (gt2reloc built without SDL)";

const char *SDL_GetError(void) { return nosdl_error; }

int SDL_Init(Uint32 flags) { (void)flags; return 0; }
void SDL_Quit(void) {}
SDL_bool SDL_SetHint(const char *name, const char *value) { (void)name; (void)value; return SDL_FALSE; }
void SDL_LogSetOutputFunction(SDL_LogOutputFunction callback, void *userdata) { (void)callback; (void)userdata; }
void SDL_free(void *mem) { free(mem); }

/* timing */
Uint32 SDL_GetTicks(void)
{
    static struct timespec t0;
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    if (!t0.tv_sec && !t0.tv_nsec) t0 = t;
    return (Uint32)((t.tv_sec - t0.tv_sec) * 1000 + (t.tv_nsec - t0.tv_nsec) / 1000000);
}
void SDL_Delay(Uint32 ms)
{
    struct timespec t = { ms / 1000, (long)(ms % 1000) * 1000000L };
    nanosleep(&t, NULL);
}
SDL_TimerID SDL_AddTimer(Uint32 interval, SDL_TimerCallback callback, void *param)
{ (void)interval; (void)callback; (void)param; return 0; }
SDL_bool SDL_RemoveTimer(SDL_TimerID id) { (void)id; return SDL_FALSE; }

/* audio: not available */
int SDL_OpenAudio(SDL_AudioSpec *desired, SDL_AudioSpec *obtained) { (void)desired; (void)obtained; return -1; }
void SDL_PauseAudio(int pause_on) { (void)pause_on; }
void SDL_CloseAudio(void) {}

/* events */
int SDL_PollEvent(SDL_Event *event) { (void)event; return 0; }
void SDL_PumpEvents(void) {}
int SDL_ShowCursor(int toggle) { (void)toggle; return 0; }

/* clipboard */
char *SDL_GetClipboardText(void) { char *s = malloc(1); if (s) *s = 0; return s; }
SDL_bool SDL_HasClipboardText(void) { return SDL_FALSE; }
int SDL_SetClipboardText(const char *text) { (void)text; return -1; }

/* video: no window, no surfaces */
SDL_Window *SDL_CreateWindow(const char *title, int x, int y, int w, int h, Uint32 flags)
{ (void)title; (void)x; (void)y; (void)w; (void)h; (void)flags; return NULL; }
void SDL_DestroyWindow(SDL_Window *window) { (void)window; }
void SDL_SetWindowIcon(SDL_Window *window, SDL_Surface *icon) { (void)window; (void)icon; }
void SDL_SetWindowSize(SDL_Window *window, int w, int h) { (void)window; (void)w; (void)h; }
SDL_Renderer *SDL_CreateRenderer(SDL_Window *window, int index, Uint32 flags) { (void)window; (void)index; (void)flags; return NULL; }
void SDL_DestroyRenderer(SDL_Renderer *renderer) { (void)renderer; }
int SDL_RenderClear(SDL_Renderer *renderer) { (void)renderer; return -1; }
int SDL_RenderCopy(SDL_Renderer *renderer, SDL_Texture *texture, const SDL_Rect *srcrect, const SDL_Rect *dstrect)
{ (void)renderer; (void)texture; (void)srcrect; (void)dstrect; return -1; }
void SDL_RenderPresent(SDL_Renderer *renderer) { (void)renderer; }
SDL_Texture *SDL_CreateTexture(SDL_Renderer *renderer, Uint32 format, int access, int w, int h)
{ (void)renderer; (void)format; (void)access; (void)w; (void)h; return NULL; }
void SDL_DestroyTexture(SDL_Texture *texture) { (void)texture; }
int SDL_UpdateTexture(SDL_Texture *texture, const SDL_Rect *rect, const void *pixels, int pitch)
{ (void)texture; (void)rect; (void)pixels; (void)pitch; return -1; }
SDL_Surface *SDL_CreateRGBSurfaceWithFormat(Uint32 flags, int width, int height, int depth, Uint32 format)
{ (void)flags; (void)width; (void)height; (void)depth; (void)format; return NULL; }
SDL_Surface *SDL_ConvertSurfaceFormat(SDL_Surface *src, Uint32 pixel_format, Uint32 flags)
{ (void)src; (void)pixel_format; (void)flags; return NULL; }
void SDL_FreeSurface(SDL_Surface *surface) { (void)surface; }
int SDL_LockSurface(SDL_Surface *surface) { (void)surface; return -1; }
void SDL_UnlockSurface(SDL_Surface *surface) { (void)surface; }
int SDL_SetPaletteColors(SDL_Palette *palette, const SDL_Color *colors, int firstcolor, int ncolors)
{ (void)palette; (void)colors; (void)firstcolor; (void)ncolors; return -1; }
SDL_RWops *SDL_RWFromMem(void *mem, int size) { (void)mem; (void)size; return NULL; }
SDL_Surface *SDL_LoadBMP_RW(SDL_RWops *src, int freesrc) { (void)src; (void)freesrc; return NULL; }
