#include <stdlib.h>
#include <unistd.h>

#include <stdio.h> /* XXX */
#include "SDL.h" /* XXX */

#include "config.h"
#include "profile.h"
#include "world.h"
#include "menu.h"

#include "io.h" /* XXX */
#include "objects.h" /* XXX */
#include "graphics.h" /* XXX */

void control_ship(Uint8 *keystate, struct player *p);
void control_weapons(Uint8 *keystate, struct player *p,
                     int *bullet_i, struct bullet *bullets);

int main(void)
{
    unsigned int fps=0, capped, last_frame_capped=0, in_menu=1;
    unsigned int start = 0, end = 0, last_frame_time = 0;
    struct world w;

    Uint8 *keystate;
    SDL_Event event;

    init_io();
    atexit(stop_io); /** XXX MOVE TO IO **/
    init_world(&w);

    /* main game loop */
    for(;;w.progress += STAR_BANNER_SPEED) {

        start = SDL_GetTicks();

        /*** XXX DEBUG */
        if (w.progress > 1000) exit(0);
        /*** XXX DEBUG */

        start_profile("update");
        start_profile("keybord input");
        /* processing keyboard input */
        SDL_PumpEvents();
        keystate = SDL_GetKeyState(NULL);
        /* checking for other SDL events */
        while(SDL_PollEvent(&event)){
            switch(event.type){
            case SDL_VIDEORESIZE:
                resize_io(event.resize.w, event.resize.h);
                break;
            case SDL_QUIT:
                exit(0);
            }
        }
        stop_profile("keybord input");

        /* menu */
        if (in_menu) {
            in_menu = show_menu(keystate, w.progress);
            continue;
        } else if (keystate[SDLK_ESCAPE]) {
            in_menu = 1;
        }

        /* clear */
        start_profile("clear screen");
        io_clear_screen();
        stop_profile("clear screen");

        /* ship controls */
        control_ship(keystate, &(w.me));
        control_weapons(keystate, &(w.me),
                        &(w.my_bullet_i), w.my_bullets);
        
        /* updating the world */
        start_profile("world update");
        if (start - w.me.last_hit > POINTS_MULTI_TIME)
            w.me.multiplier--;
        if (w.me.multiplier < 1)
            w.me.multiplier = 1;
        update_world(&w, last_frame_time);
        stop_profile("world update");
        stop_profile("update");

        /* rendering everything */
        start_profile("render");
        draw_world(&w);
        if (! last_frame_capped)
            io_set_color(0xff, 0, 0);
        io_printf(1, 1, "%04d FPS", fps);
        io_reset_color();
        stop_profile("render");
        draw_profile();
        update_io();

        /* frame rate limiter */
        last_frame_capped = 0;
        do {
            end = SDL_GetTicks();
            last_frame_time = end - start;
            fps = 1000 / last_frame_time;
            capped = 0;
            if (fps > MAX_FPS) {
                capped = 1;
                last_frame_capped = 1;
                usleep((1000/MAX_FPS - last_frame_time)*1000);
            }
        } while(capped);
    }
}

void control_ship(Uint8 *keystate, struct player *p)
{
    if (keystate[SDLK_w] || keystate[SDLK_UP])    p->y -= 1;
    if (keystate[SDLK_s] || keystate[SDLK_DOWN])  p->y += 1;
    if (keystate[SDLK_a] || keystate[SDLK_LEFT])  p->x -= 1;
    if (keystate[SDLK_d] || keystate[SDLK_RIGHT]) p->x += 1;
    if (p->x < 0) p->x = 0;
    if (p->x > X_MAX-3) p->x = X_MAX-3;
    if (p->y < 0) p->y = 0;
    if (p->y > Y_MAX-3) p->y = Y_MAX-2;
}

void control_weapons(Uint8 *keystate, struct player *p,
                     int *bullet_i, struct bullet *bullets)
{
    struct bullet *b;
    b = &bullets[*bullet_i];
    if (keystate[SDLK_SPACE] && p->weapon_heat < MAX_WEAPON_HEAT) {
        b->shot = 1;
        b->x = p->x;
        b->y = p->y;
        (*bullet_i)++;
        p->weapon_heat++;
    } else {
        /* weapon cool-down */
        if (rand() % 2 == 0) {
            p->weapon_heat--;
        }
        if (p->weapon_heat < 0) p->weapon_heat = 0;
    }
}



