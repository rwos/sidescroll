#include <stdlib.h>

#include "config.h"
#include "objects.h"
#include "io.h"

#include "graphics.h"

int explosion_size;

void set_explosion_size(int n)
{
    explosion_size = n;
}

void draw_explosion(struct explosion *e)
{
    int i, j, oldx, oldy;
    char c = ' ';
    
    if (e->progress) {
        oldx = e->x;
        oldy = e->y;
        for (j=0; j<30; j++) {
            e->x = oldx;
            e->y = oldy;
            for (i=0; i<explosion_size; i++) {
                switch (rand() % 4) {
                case 0: e->x += 2; break;
                case 1: e->x -= 2; break;
                case 2: e->y++; break;
                case 3: e->y--; break;
                }
                switch (rand() % 7) {
                case 0: c='*'; break;
                case 1: c='~'; break;
                case 2: c='/'; break;
                case 3: c='x'; break;
                case 4:
                case 5:
                case 6: c=' '; break;
                }
                io_set_color(e->progress, (i*5) % e->progress, 0);
                io_putc(e->x, e->y, c);
            }
        }
        e->x = oldx;
        e->y = oldy;
        io_reset_color();
    }
}

void draw_ship(struct player *p)
{
    io_set_color((100 - p->health)*2, 55 + (p->health*2), 0);
    io_puts(p->x, p->y,   " -|\\");
    io_puts(p->x, p->y+1, ">]]]]]>");
    io_puts(p->x, p->y+2, " -|/");
    io_reset_color();
}

void draw_enemy(struct enemy *e)
{
    if (e->health) {
        if ((int)e % 5) {
            io_set_color(0x77, (int)(2.5 * e->health), 155 + (int)e % 100);
            io_puts(e->x, e->y,   "<<[[[\\");
            io_puts(e->x, e->y+1, "  (XXXX[");
            io_puts(e->x, e->y+2, "<<[[[/");
        } else {
            io_set_color(155 + (int)e % 100, (int)(2.5 * e->health), 0x33);
            io_puts(e->x, e->y,   "  /|- /");
            io_puts(e->x, e->y+1, "<[[[[<");
            io_puts(e->x, e->y+2, "  \\|- \\");
        }
        io_reset_color();
    }
}

void draw_bullet(struct bullet *b)
{
    if (b->shot) {
        io_putc(b->x+1, b->y+1, '=');
    }
}

void draw_enemy_bullet(struct bullet *b)
{
    if (b->shot) {
        io_set_color(0xff, 0xff, 0x00);
        io_puts(b->x+1, b->y,   "~~");
        io_puts(b->x+1, b->y+2, "~~");
        io_reset_color();
    }
}

void draw_background(unsigned int progress)
{
    int x, y, r, paral, x_step, y_step;

    x_step = X_MAX / STAR_BANNER_X_STARS;
    if (x_step == 0) x_step = 1;
    y_step = Y_MAX / STAR_BANNER_Y_STARS;
    if (y_step == 0) y_step = 1;

    for (paral=0; paral<STAR_BANNER_PARALAX; paral++) {
        io_set_color(
            (180 / STAR_BANNER_PARALAX) * (paral + 1),
            (180 / STAR_BANNER_PARALAX) * (paral + 1),
            (180 / STAR_BANNER_PARALAX) * (paral + 1));
        for (y=0; y<Y_MAX/y_step; y++) {
            for (x=0; x<X_MAX/x_step; x++) {
                srand(progress/(STAR_BANNER_PARALAX-paral)+x+(Y_MAX*y));
                if (! (rand() % (20*(paral+1)*(paral+1)))) {
                    io_putc(x*x_step, y*y_step, '.');
                }
            }
        }
        io_reset_color();
    }
}

