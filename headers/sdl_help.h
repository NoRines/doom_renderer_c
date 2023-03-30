#pragma once

#include <SDL2/SDL.h>

int sdl_help_init(int w, int h, const char* s);
void sdl_help_quit();

SDL_Renderer *sdl_help_renderer();
