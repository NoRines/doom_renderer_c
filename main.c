#include "sdl_help.h"
#include "input.h"
#include "angle.h"
#include "vec2.h"
#include "player.h"
#include "wad.h"
#include "map.h"
#include "draw2d.h"
#include "draw3d.h"

#include <stdlib.h>
#include <assert.h>

// #define WIDTH 320
// #define HEIGHT 240
// #define WINDOW_SCALE 3

#define WIDTH 800
#define HEIGHT 600
#define WINDOW_SCALE 1

void run() {
    player_t player;
    player.id = 1;
    player_set_start(&player);

    double move_speed = 5.0;

    SDL_Renderer *prenderer = sdl_help_renderer();

    bool in_3d = false;

    do {
        bool previous_escape = input_key(SDL_SCANCODE_ESCAPE);
        bool zero = input_key(SDL_SCANCODE_0);
        input_process();

        if (input_key(SDL_SCANCODE_0) && !zero) {
            int a = 0;
        }

        if (input_key(SDL_SCANCODE_ESCAPE) && !previous_escape) {
            in_3d = !in_3d;
        }

        if (input_key(SDL_SCANCODE_LSHIFT)) {
            move_speed = 5.0 / 15.0;
        } else {
            move_speed = 5.0;
        }

        if (input_key(SDL_SCANCODE_UP)) {
            player_move_forward(&player, move_speed);
        } else if (input_key(SDL_SCANCODE_DOWN)) {
            player_move_forward(&player, -move_speed);
        }
        if (input_key(SDL_SCANCODE_LEFT)) {
            player.angle = angle_add(player.angle, 5.0 * ANG1);
        } else if (input_key(SDL_SCANCODE_RIGHT)) {
            player.angle = angle_sub(player.angle, 5.0 * ANG1);
        }

        SDL_SetRenderDrawColor(prenderer, 0, 0, 0, 255);
        SDL_RenderClear(prenderer);

        if (!in_3d) {
            /* Render map and player in 2d */
            draw2d_automap();
            draw2d_visible_automap(&player);
            draw2d_player(&player);
        } else {
            draw3d_view(&player);
        }

        SDL_RenderPresent(prenderer);

        SDL_Delay(20);
    } while (input_window_open());
}

int init_game() {
    int error = wad_load("wads\\DOOM.WAD");

    if (!error) {
        error = map_create("E1M3");
    }

    wad_unload();

    draw2d_init();
    draw3d_init();

    return error;
}

void free_all_resources() {
    printf("INFO: Freeing global resources.\n");
    printf("        map_destroy()\n");
    map_destroy();
    printf("        draw3d_quit()\n");
    draw3d_quit();
}

int main(int argc, char *argv[]) {
    atexit(sdl_help_quit);
    atexit(free_all_resources);
    int error = sdl_help_init(WIDTH * WINDOW_SCALE, HEIGHT * WINDOW_SCALE, "my game engine");
    SDL_RenderSetLogicalSize(sdl_help_renderer(), WIDTH, HEIGHT);

    if (!error) {
        error = init_game();
    }

    if (!error) {
        run();
    }

    return error;
}