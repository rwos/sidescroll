#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>

#include "SDL.h"
#include "SDL_ttf.h"    

#include "io.h"

int SCREEN_WIDTH  = 800;
int SCREEN_HEIGHT = 600;

#define FONT_SIZE 12
#define BLOCK_SIZE_X 7
#define BLOCK_SIZE_Y 12

int X_MAX;
int Y_MAX;

SDL_Surface *screen;
TTF_Font *font;

#define FG_COLOR_R 0x33
#define FG_COLOR_G 0xdd
#define FG_COLOR_B 0x33
SDL_Color fg_color = {FG_COLOR_R, FG_COLOR_G, FG_COLOR_B, 0};

void init_io(void)
{
    /* init SDL */
    if (SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
        exit(1);
    }
    /* init SDL ttf */
    if (TTF_Init() < 0) {
        fprintf(stderr, "Unable to init SDL_ttf: %s\n", TTF_GetError());
        exit(1);
    }
    /* load font */
    font = TTF_OpenFont("font.ttf", FONT_SIZE);
    if (font == NULL) {
        fprintf(stderr, "Unable to load font: %s\n", TTF_GetError());
        exit(1);
    }
    /* init SDL screen */
    resize_io(SCREEN_WIDTH, SCREEN_HEIGHT);
}

void stop_io(void)
{
    SDL_Quit();
}

void update_io(void)
{
    SDL_UpdateRect(screen, 0, 0, 0, 0); /* entire screen */
}

void resize_io(int w, int h)
{
    SCREEN_WIDTH  = w;
    SCREEN_HEIGHT = h;
    X_MAX = (SCREEN_WIDTH / BLOCK_SIZE_X);
    Y_MAX = (SCREEN_HEIGHT / BLOCK_SIZE_Y);
    screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 24,
                              SDL_SWSURFACE|SDL_ASYNCBLIT|SDL_RESIZABLE);
    if (screen == NULL) {
        fprintf(stderr, "Unable to set %dx%d video: %s\n",
                SCREEN_WIDTH, SCREEN_HEIGHT,
                SDL_GetError());
        exit(1);
    }
}

void io_puts(int x, int y, const char *s)
{
    SDL_Surface *txt_surface;
    SDL_Rect dest;

    txt_surface = TTF_RenderText_Solid(font, s, fg_color);
    dest.x = x * BLOCK_SIZE_X;
    dest.y = y * BLOCK_SIZE_Y;
    dest.w = BLOCK_SIZE_X * strlen(s);
    dest.h = BLOCK_SIZE_Y;
    SDL_BlitSurface(txt_surface, NULL, screen, &dest);
    SDL_FreeSurface(txt_surface);
}

void io_printf(int x, int y, const char *format, ...)
{
    va_list args;
    char buf[1024];

    va_start(args, format);
    vsnprintf(buf, 1024, format, args);
    io_puts(x, y, buf);
    va_end(args);
}

void io_set_color(int r, int g, int b)
{
    fg_color.r = r;
    fg_color.b = b;
    fg_color.g = g;
}

void io_reset_color(void)
{
    fg_color.r = FG_COLOR_R;
    fg_color.b = FG_COLOR_B;
    fg_color.g = FG_COLOR_G;
}

void io_putc(int x, int y, char c)
{
    char buf[2] = {'\0', '\0'};

    buf[0] = c;
    io_puts(x, y, buf);    
}

void io_clear_screen(void)
{
    SDL_FillRect(screen, 0, 0);
}

