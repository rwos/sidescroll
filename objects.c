#include <stdlib.h>

#include "config.h"
#include "io.h"
#include "graphics.h"
#include "SDL.h" /* XXX only because of SDL_GetTicks... */

#include "objects.h"

void update_enemy(struct enemy *e,
                  int explosion_i, struct explosion *explosions)
{
    if (! e->health) {
        /* randomly wake up enemies */
        if (rand() % 50 == 0) {
            e->health = 100;
        }
        /* start point */
        e->x = X_MAX+20;
        e->y = rand() % Y_MAX;
        e->y_dir = 0;
    }
    if (e->health) {
        /* enemy already awake -> movement */
        switch(rand() % 10) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4: e->x--; break;

        case 6:
        case 7:
            if (e->y_dir == 1) {
                e->y++;
            } else {
                e->y_dir = 1;
            }
            break;
        case 8:
        case 9:
            if (e->y_dir == -1) {
                e->y--;
            } else {
                e->y_dir = -1;
            }
            break;
        }
    }
    /* kill when outside of field */
    if (e->y < -3 || e->y > Y_MAX || e->x < -5 || e->x > X_MAX+50) {
        e->health = 0;
        start_explosion(e->x, e->y, 30, 150, explosion_i, explosions);
    }
}

void update_enemy_bullet(struct bullet *b, struct player *p,
                         int explosion_i, struct explosion *explosions)
{
    if (b->shot) {
        b->x -= ENEMY_BULLET_SPEED;
        if (b->x < 0) {
            b->shot = 0;
        }
        /* test player hit */
        if ((b->y > p->y-4 && b->y < p->y+1)
        &&  (b->x > p->x-1 && b->x < p->x+3)) {
            /* HIT! */
            p->health -= 15;
            b->shot = 0;
            (p->x) -= 3;
            start_explosion(b->x, b->y, 15, 100, explosion_i, explosions);
        }
    }
}

void control_enemy_weapons(struct enemy *e, int *bullet_i, struct bullet *b)
{
    if (e->health && rand() % 80 == 0) {
        b->shot = 1;
        b->x = e->x;
        b->y = e->y;
        (*bullet_i)++;
    }
}

void start_explosion(int x, int y, int size, int length,
                     int explosion_i, struct explosion *explosions)
{
    if (explosion_i >= EXPLOSION_MAX) {
        explosion_i = 0;
    }
    set_explosion_size(size);
    explosions[explosion_i].progress = length + rand() % 100;
    explosions[explosion_i].x = x;
    explosions[explosion_i].y = y;
    explosion_i += 1;
}

void update_my_bullet(struct bullet *b, struct player *p,
                      struct enemy *enemies,
                      int explosion_i, struct explosion *explosions,
                      unsigned int time_slice)
{
    int i;
    unsigned int current;
    struct enemy *e;

    if (b->shot) {
        b->x += PLAYER_BULLET_SPEED * time_slice;
        if (b->x > X_MAX) {
            b->shot = 0;
        }
        /* test enemy hits */
        for (i=0; i<ENEMY_MAX; i++) {
            e = &enemies[i];
            if (! e->health) continue;
            if ((b->y > e->y-4 && b->y < e->y+1)
            &&  (b->x > e->x-1 && b->x < e->x+3)) {
                /* HIT! */
                current = SDL_GetTicks();
                if (current - p->last_hit < POINTS_MULTI_TIME)
                    p->multiplier += 1;
                p->last_hit = current;
                p->points += (p->multiplier*POINTS_PER_HIT);
                b->shot = 0;
                e->health -= DAMAGE;
                start_explosion(b->x, b->y, 15, 80,
                                explosion_i, explosions);
                if (e->health < 0) {
                    e->health = 0;
                    start_explosion(b->x, b->y, 50, 80,
                                    explosion_i, explosions);
                }
            }
        }
    }
}

void update_explosion(struct explosion *e)
{
    if (e->progress) {
        e->progress -= 20;
        if (e->progress < 0) {
            e->progress = 0;
        }
    }
}

