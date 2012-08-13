#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "objects.h"

void draw_explosion(struct explosion *e);

void draw_ship(struct player *p);

void draw_enemy(struct enemy *e);

void draw_bullet(struct bullet *b);
void draw_enemy_bullet(struct bullet *b);

void draw_background(unsigned int progress);

void set_explosion_size(int n);

#endif /* GRAPHICS_H */

