#ifndef WORLD_H
#define WORLD_H

#include "objects.h"
#include "config.h"

struct world {
    struct player    me;
    struct player    mate;
    struct enemy     enemies[ENEMY_MAX];
    struct bullet    my_bullets[MY_BULLET_MAX];
    struct bullet    mate_bullets[MY_BULLET_MAX];
    struct bullet    enemy_bullets[ENEMY_BULLET_MAX];
    struct explosion explosions[EXPLOSION_MAX];
    unsigned int progress;
    /* indices */
    int enemy_i;
    int my_bullet_i;
    int enemy_bullet_i;
    int explosion_i;
};

void init_world  (struct world *w);
void update_world(struct world *w);
void draw_world  (struct world *w);
void merge_worlds(struct world *w1, struct world *w2, enum mode mo);

#endif /* WORLD_H */

