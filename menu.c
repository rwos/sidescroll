#include <stdio.h>
#include "SDL.h"

#include "io.h"
#include "objects.h"
#include "graphics.h"
#include "config.h"

#include "menu.h"

/**
 * XXX - COMPLETE REWRITE needed - our APIs are bad and we should feel bad.
 */

enum button {UNUSED=-1, START_SP, GOTO_CLIENT, GOTO_SERVER, EXIT};
enum submenu {MAIN_MENU, CLIENT_MENU, SERVER_MENU};

void draw_menu_head(int progress, int *offset);
void draw_menu_bottom(enum button selected_option,
                      enum submenu current_submenu, int *offset);

void draw_main_menu(enum button selected_option, int *offset);
void draw_server_menu(enum button selected_option, int *offset);
void draw_client_menu(enum button selected_option, int *offset);

void centered(int *y, char *s);
void button(int *y, char *text, int selected);

int show_menu(Uint8 *keystate, int progress)
{
    static enum submenu in_submenu = MAIN_MENU;
    static enum button selection = START_SP;
    static int down = 0;
    static int up = 0;
    static int fire = 0;
    int menu_offset = Y_MAX/2-10;

    /* keyboard handling */
    if (keystate[SDLK_DOWN] || keystate[SDLK_s]) {
        down = 1;
    } else if (down) {
        selection += 1;
        down = 0;
    }
    if (keystate[SDLK_UP] || keystate[SDLK_w]) {
        up = 1;
    } else if (up) {
        selection -= 1;
        up = 0;
    }
    if (selection > 3) selection = 0;
    if (selection < 0) selection = 3;

    /* drawing current submenu */
    io_clear_screen();
    draw_menu_head(progress, &menu_offset);
    switch (in_submenu) {
    case MAIN_MENU:
        draw_main_menu(selection, &menu_offset);
        break;
    case CLIENT_MENU:
        draw_client_menu(selection, &menu_offset);
        break;
    case SERVER_MENU:
        draw_server_menu(selection, &menu_offset);
        break;
    }
    draw_menu_bottom(selection, in_submenu, &menu_offset);
    update_io();

    /* selection handling */
// XXX must depend on submenu
    if (keystate[SDLK_SPACE] || keystate[SDLK_RETURN]) {
        fire = 1;
    } else if (fire) {
        fire = 0;
        switch (selection) {
            case START_SP:
                return 0; /* start game */
            case GOTO_CLIENT:
                break; // XXX XXX XXX XXX XXX
                selection = 0; /* XXX CLIENT_ENTER_IP */
                in_submenu = CLIENT_MENU;
                break;
            case GOTO_SERVER:
                break; // XXX XXX XXX XXX XXX
                selection = 0; /* XXX START_SERVER */
                in_submenu = SERVER_MENU;
                break;
            case EXIT:
                if (in_submenu == MAIN_MENU)
                    exit(0);
                else
                    in_submenu = MAIN_MENU;
                break;
        }
    }
    return 1; // keep in menu
}

void draw_menu_head(int progress, int *offset)
{
    struct player pseudo_player;

    pseudo_player.x = X_MAX/2-2;
    pseudo_player.y = 10;
    pseudo_player.health = 100;

    draw_background(progress);
    draw_ship(&pseudo_player);
    io_set_color(0xff, 0, 0);
    centered(offset, GAME_NAME);
    io_reset_color();
    *offset += 2;
    centered(offset, "control your ship using the ARROW KEYS or with");
    *offset += 1;
    centered(offset, "[W]");
    centered(offset, "[A][S][D]");
    *offset += 1;
    centered(offset, "shoot with [SPACE]");
    *offset += 3;
}

void draw_menu_bottom(enum button selected_option,
                      enum submenu current_submenu, int *offset)
{
    *offset += 2;
    if (current_submenu == MAIN_MENU)
        button(offset, "EXIT", (selected_option == EXIT));
    else
        button(offset, "BACK TO MAIN MENU", (selected_option == EXIT));
}

void draw_main_menu(enum button selected_option, int *offset)
{
    button(offset, "SINGLE PLAYER",      (selected_option == START_SP));
    button(offset, "MULTIPLAYER CLIENT (non-worky)", (selected_option == GOTO_CLIENT));
    button(offset, "MULTIPLAYER SERVER (non-worky)", (selected_option == GOTO_SERVER));
}

void draw_server_menu(enum button selected_option, int *offset)
{
    button(offset, "START SERVER",      (selected_option == START_SP));
}

void draw_client_menu(enum button selected_option, int *offset)
{
    button(offset, "XXXXXXXXXXXXXXXXXXXXXXSINGLE PLAYER",      (selected_option == START_SP));
    button(offset, "MULTIPLAYER CLIENT", (selected_option == GOTO_CLIENT));
    button(offset, "MULTIPLAYER SERVER", (selected_option == GOTO_SERVER));
}

void button(int *y, char *text, int selected)
{
    int i;
    char buf[1024];
    if (selected)
        io_set_color(0xff, 0xff, 0);
    else
        io_set_color(0xff, 0xff, 0xff);
    // top
    centered(y, ",--------------------------------------,");
    // middle
    centered(y, text);
    *y -= 1;
    if (selected)
        centered(y, "| >>>                              <<< |");
    else
        centered(y, "|                                      |");
    // bottom
    centered(y, "'--------------------------------------'");
    // margin
    *y += 1;
    io_reset_color();
}

void centered(int *y, char *s)
{
    int x;
    x = X_MAX/2 - strlen(s)/2;
    io_puts(x, *y, s);
    *y += 1;
}

