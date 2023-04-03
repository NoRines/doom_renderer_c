#pragma once

#include <SDL2/SDL.h>

int sdl_help_init(int w, int h, const char* s);
void sdl_help_quit();

SDL_Renderer *sdl_help_renderer();

void sdl_help_start_frame();
void sdl_help_finish_frame();


int32_t sdl_help_screen_buffer_h();
int32_t sdl_help_screen_buffer_w();

void sdl_help_draw_vertical_line(int32_t x, int32_t start, int32_t end, uint8_t color);