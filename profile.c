#include <string.h>
#include "SDL.h"

#include "io.h"

#include "profile.h"

struct prof_datum {
    char descr[1024];
    int depth;
    int running;
    int ms;
};

#define PROF_DATA_COUNT 512
struct prof_datum prof_data[PROF_DATA_COUNT];
int prof_data_i;

int current_depth;

void reset_profile();

void start_profile(const char *descr)
{
    static int first_run = 1;
    if (first_run) {
        reset_profile();
        first_run = 0;
    }
    memcpy(&prof_data[prof_data_i].descr, descr, strlen(descr)+1);
    prof_data[prof_data_i].depth = current_depth;
    prof_data[prof_data_i].running = 1;
    prof_data[prof_data_i].ms = SDL_GetTicks();
    prof_data_i += 1;
    if (prof_data_i >= PROF_DATA_COUNT) {
        prof_data_i = 0; /* wrap index around */
    }
    current_depth += 1;
}

void stop_profile(const char *descr)
{
    int i;
    /* find and stop prof_datum */
    for (i=0; i<PROF_DATA_COUNT; i++) {
        if (strcmp(prof_data[i].descr, descr) == 0) {
            prof_data[i].running = 0;
            prof_data[i].ms = SDL_GetTicks() - prof_data[i].ms;
            break;
        }
    }
    current_depth -= 1;
}

void reset_profile()
{
    int i;
    for (i=0; i<PROF_DATA_COUNT; i++) {
        prof_data[i].descr[0] = '\0';
        prof_data[i].depth = 0;
        prof_data[i].running = 0;
        prof_data[i].ms = 0;
    }
    current_depth = 0;
    prof_data_i = 0;
}


void draw_profile()
{
    int i, y_off=3;
    for (i=0; i<PROF_DATA_COUNT; i++) {
        if (prof_data[i].descr[0] != '\0' && (! prof_data[i].running)) {
            io_puts(prof_data[i].depth * 2 + 2, y_off, prof_data[i].descr);
            io_printf(prof_data[i].depth * 2 + 20, y_off, "% 4d ms",
                      prof_data[i].ms);
            y_off += 1;
        }
    }
    reset_profile();
}

