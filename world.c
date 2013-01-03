#include <string.h>

#include "profile.h"
#include "io.h"
#include "graphics.h"

#include "world.h"

void init_world(struct world *w)
{
    int i;
    /* player */
    w->me.health      = 100;
    w->me.x           = 5;
    w->me.y           = 20;
    w->me.points      = 0;
    w->me.multiplier  = 1;
    w->me.multiplier_max  = 0;
    w->me.last_hit    = 1;
    w->me.weapon_heat = 0;
    /* enemies */
    for (i=0; i<ENEMY_MAX; i++) {
        w->enemies[i].health = 0;
    }
    /* bullets */
    for (i=0; i<MY_BULLET_MAX; i++) {
        w->my_bullets[i].shot = 0;
    }
    for (i=0; i<ENEMY_BULLET_MAX; i++) {
        w->enemy_bullets[i].shot = 0;
    }
    /* explosions */
    for (i=0; i<EXPLOSION_MAX; i++) {
        w->explosions[i].progress = 0;
    }
    /* world progress */
    w->progress = 0;
    /* indices */
    w->enemy_i        = 0;
    w->my_bullet_i    = 0;
    w->enemy_bullet_i = 0;
    w->explosion_i    = 0;
}

void update_world(struct world *w, unsigned int time_slice)
{
    int i, oldx, oldy, oldpoints;
    /* roll-around indices */
    if (w->enemy_i >= ENEMY_MAX) w->enemy_i = 0;
    if (w->my_bullet_i >= MY_BULLET_MAX) w->my_bullet_i = 0;
    if (w->enemy_bullet_i >= ENEMY_BULLET_MAX) w->enemy_bullet_i = 0;
    if (w->explosion_i >= EXPLOSION_MAX) w->explosion_i = 0;
    /* update objects */
    for (i=0; i<ENEMY_MAX; i++) {
        update_enemy(&(w->enemies[i]), w->explosion_i, w->explosions);
        control_enemy_weapons(&(w->enemies[i]), &(w->enemy_bullet_i),
                              &(w->enemy_bullets[w->enemy_bullet_i]));
    }
    for (i=0; i<MY_BULLET_MAX; i++) {
        update_my_bullet(&(w->my_bullets[i]), &(w->me),
                         w->enemies,
                         w->explosion_i, w->explosions,
                         time_slice);
    }
    for (i=0; i<ENEMY_BULLET_MAX; i++) {
        update_enemy_bullet(&(w->enemy_bullets[i]), &(w->me),
                            w->explosion_i, w->explosions);
    }
    for (i=0; i<EXPLOSION_MAX; i++) {
        update_explosion(&(w->explosions[i]));
    }
    /* player health check */
    if (w->me.health < 1) {
        /* player dead */
        oldx = w->me.x;
        oldy = w->me.y;
        oldpoints = w->me.points;
        init_world(w);
        w->me.x = oldx;
        w->me.y = oldy;
        w->me.points = oldpoints;
        start_explosion(w->me.x+3, w->me.y-3, 500, 500,
                        w->explosion_i, w->explosions);
        w->me.health = 100;
        w->me.multiplier = 1;
        w->me.points -= LIVE_COST;
    }
}

void draw_world(struct world *w)
{
    int i;
    /* background */
    start_profile("background");
    draw_background(w->progress);
    stop_profile("background");
    /* player */
    start_profile("player ship");
    draw_ship(&(w->me));
    stop_profile("player ship");
    /* objects */
    start_profile("player bullets");
    for (i=0; i<MY_BULLET_MAX; i++) {
        draw_bullet(&(w->my_bullets[i]));
    }
    stop_profile("player bullets");
    start_profile("enemy bullets");
    for (i=0; i<ENEMY_BULLET_MAX; i++) {
        draw_enemy_bullet(&(w->enemy_bullets[i]));
    }
    stop_profile("enemy bullets");
    start_profile("enemy ships");
    for (i=0; i<ENEMY_MAX; i++) {
        draw_enemy(&(w->enemies[i]));
    }
    stop_profile("enemy ships");
    start_profile("explosions");
    for (i=0; i<EXPLOSION_MAX; i++) {
        draw_explosion(&(w->explosions[i]));
    }
    stop_profile("explosions");

    /* HUD */
    start_profile("HUD");
    if (w->me.weapon_heat > 50)
        io_set_color(0xff, 0xff, 0x00);
    if (w->me.weapon_heat > 90)
        io_set_color(0xff, 0x00, 0x00);
    io_printf(1, Y_MAX-2, "LASER HEAT: % 3d%%", w->me.weapon_heat);
    io_reset_color();

    if (w->me.health < 50)
        io_set_color(0xff, 0xff, 0x00);
    if (w->me.health < 30)
        io_set_color(0xff, 0x00, 0x00);
    io_printf(X_MAX-15, 2, "HEALTH: %03d%%", w->me.health);
    io_reset_color();

    if (w->me.points < 0)
        io_set_color(0xff, 0x00, 0x00);
    io_printf(X_MAX-15, 1, "%09d x %d", w->me.points, w->me.multiplier);
    io_reset_color();
    stop_profile("HUD");
}

