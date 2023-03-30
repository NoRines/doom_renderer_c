#include "input.h"

#include <SDL2/SDL.h>

#define MAX_KEYS 256
static bool key_state[MAX_KEYS] = {0};
static bool window_open = false;

static void set_key_state(unsigned int scancode, bool state) {
    if (scancode < MAX_KEYS) {
        key_state[scancode] = state;
    }
}

void input_process() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
        case SDL_QUIT:
            window_open = false;
            break;

        case SDL_KEYDOWN:
            set_key_state(e.key.keysym.scancode, true);
            break;
        case SDL_KEYUP:
            set_key_state(e.key.keysym.scancode, false);
            break;

        case SDL_WINDOWEVENT:
            if (e.window.event == SDL_WINDOWEVENT_SHOWN) {
                window_open = true;
            }
            break;
        
        default:
            break;
        }
    }
}

bool input_window_open() {
    return window_open;
}

bool input_key(unsigned int scancode) {
    if (scancode >= MAX_KEYS) {
        return false;
    }
    return key_state[scancode];
}

