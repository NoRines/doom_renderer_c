#include "sdl_help.h"


static SDL_Window *p_window = NULL;
static SDL_Renderer *p_renderer = NULL;
// 2 SDL_Surface + 1 SDL_Texture

int sdl_help_init(int w, int h, const char* s) {
    int error = SDL_Init(SDL_INIT_VIDEO);

    if (error != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, SDL_GetError());
        return error;
    }
    SDL_Log("SDL Initialized");

    p_window = SDL_CreateWindow(
        s,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        w, h,
        SDL_WINDOW_ALLOW_HIGHDPI
    );

    if (!p_window) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, SDL_GetError());
        return -1;
    }
    SDL_Log("SDL_Window created");

    p_renderer = SDL_CreateRenderer(p_window, -1, SDL_RENDERER_ACCELERATED);

    if (!p_renderer) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, SDL_GetError());
        return -1;
    }
    SDL_Log("SDL_renderer created");

    SDL_RenderSetLogicalSize(p_renderer, w, h);

    return 0;
}


void sdl_help_quit() {
    SDL_Log("Cleaning SDL");
    SDL_DestroyRenderer(p_renderer);
    SDL_DestroyWindow(p_window);
    SDL_Quit();
}

SDL_Renderer *sdl_help_renderer() {
    return p_renderer;
}
