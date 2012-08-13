#ifndef OBJECTS_H
#define OBJECTS_H

struct bullet {
    int shot;
    int x;
    int y;
};

struct enemy {
    int health;
    int y_dir;
    int x;
    int y;
};

struct player {
    int health;
    int x;
    int y;
    int points;
    unsigned int multiplier;
    unsigned int multiplier_max;
    unsigned int last_hit;
    int weapon_heat;
};

struct explosion {
    int progress;
    int x;
    int y;
};


void update_explosion(struct explosion *e);

void update_enemy(struct enemy *e,
                  int explosion_i, struct explosion *explosions);
void update_enemy_bullet(struct bullet *b, struct player *p,
                         int explosion_i, struct explosion *explosions);
void update_my_bullet(struct bullet *b, struct player *p,
                      struct enemy *enemies,
                      int explosion_i, struct explosion *explosions,
                      unsigned int time_slice);

void control_enemy_weapons(struct enemy *e, int *bullet_i, struct bullet *b);

void start_explosion(int x, int y, int size, int length,
                     int explosion_i, struct explosion *explosions);

#endif /* OBJECTS_H */

