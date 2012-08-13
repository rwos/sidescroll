extern "C" {
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include <stdio.h> /* XXX */
#include "SDL.h" /* XXX */

#include "config.h"
#include "world.h"
#include "menu.h"

#include "io.h" /* XXX */
#include "objects.h" /* XXX */
#include "graphics.h" /* XXX */
}
#include "serv.hpp"

void control_ship(Uint8 *keystate, struct player *p);
void control_weapons(Uint8 *keystate, struct player *p,
                     int *bullet_i, struct bullet *bullets);

int main(int argc, char *argv[])
{
    int coopFd;
    enum mode mo;

    unsigned int fps=0, capped, last_frame_capped=0, start, end, in_menu=1;
    struct world w, coop;

    pthread_t t;
	pthread_create(&t, NULL, createServ, NULL);

//	coopFd = createServ();
    //init_world(&w);

    /* main game loop */
    for(;;w.progress += STAR_BANNER_SPEED) {

        start = SDL_GetTicks();

        update_players();

        sleep(1);
        /* frame rate limiter */
        last_frame_capped = 0;
        do {
            end = SDL_GetTicks();
            fps = 1000 / (end-start);
            capped = 0;
            if (fps > MAX_FPS) {
                capped = 1;
                last_frame_capped = 1;
                usleep((1000/MAX_FPS - (end-start))*1000);
                io_printf(10, 1, "CAP", fps);
            }
        } while(capped);
    }
}



