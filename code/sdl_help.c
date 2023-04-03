#include "sdl_help.h"
#include "wad.h"

static SDL_Window *p_window = NULL;
static SDL_Renderer *p_renderer = NULL;
static SDL_Texture *p_texture = NULL;
static SDL_Surface *p_screen_buffer = NULL;
static SDL_Surface *p_rgb_buffer = NULL;

typedef struct {
    SDL_Color colors[N_PALETTE_COLORS];
} sdl_palette_t;

static uint32_t n_palettes = 0;
static sdl_palette_t *p_palettes = NULL;

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

    p_screen_buffer = SDL_CreateRGBSurface(0, w, h, 8, 0, 0, 0, 0);
    if (!p_screen_buffer) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, SDL_GetError());
        return -1;
    }
    SDL_FillRect(p_screen_buffer, NULL, 0);
    SDL_Log("SDL_Surface screen buffer created");

    uint32_t pixel_format = SDL_GetWindowPixelFormat(p_window);
    int32_t bpp;
    uint32_t rmask, gmask, bmask, amask;
    SDL_PixelFormatEnumToMasks(pixel_format, &bpp, &rmask, &gmask, &bmask, &amask);

    p_rgb_buffer = SDL_CreateRGBSurface(0, w, h, bpp, rmask, gmask, bmask, amask);
    if (!p_rgb_buffer) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, SDL_GetError());
        return -1;
    }
    SDL_Log("SDL_Surface rgb buffer created");

    p_texture = SDL_CreateTexture(p_renderer, pixel_format, SDL_TEXTUREACCESS_STREAMING, w, h);
    if (!p_texture) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, SDL_GetError());
        return -1;
    }
    SDL_Log("SDL_Texture created");

    palette_t *temp_palettes = NULL;
    if (wad_read_palettes(&temp_palettes, &n_palettes)) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load palettes from wad.");
        return -1;
    }
    p_palettes = calloc(n_palettes, sizeof *p_palettes);
    for (uint32_t i = 0; i < n_palettes; i++) {
        for (uint32_t j = 0; j < N_PALETTE_COLORS; j++) {
            p_palettes[i].colors[j].a = temp_palettes[i].colors[j].a;
            p_palettes[i].colors[j].r = temp_palettes[i].colors[j].r;
            p_palettes[i].colors[j].g = temp_palettes[i].colors[j].g;
            p_palettes[i].colors[j].b = temp_palettes[i].colors[j].b;
        }
    }
    free(temp_palettes);
    SDL_Log("Loaded palettes");

    return 0;
}


void sdl_help_quit() {
    SDL_Log("Cleaning SDL");
    free(p_palettes);
    SDL_DestroyTexture(p_texture);
    SDL_FreeSurface(p_rgb_buffer);
    SDL_FreeSurface(p_screen_buffer);
    SDL_DestroyRenderer(p_renderer);
    SDL_DestroyWindow(p_window);
    SDL_Quit();
}

SDL_Renderer *sdl_help_renderer() {
    return p_renderer;
}


void sdl_help_start_frame() {
    SDL_FillRect(p_screen_buffer, NULL, 0);
}

void sdl_help_finish_frame() {
    SDL_SetPaletteColors(p_screen_buffer->format->palette, p_palettes[0].colors, 0, N_PALETTE_COLORS);

    SDL_BlitSurface(p_screen_buffer, NULL, p_rgb_buffer, NULL);

    uint8_t *pixels = NULL;
    int32_t pitch = 0;
    SDL_LockTexture(p_texture, NULL, (void**)&pixels, &pitch);

    int32_t i = 0;
    while (i < p_rgb_buffer->h) {
        SDL_memcpy(pixels + i * pitch, (uint8_t*)p_rgb_buffer->pixels + i * p_rgb_buffer->pitch, 4 * p_rgb_buffer->w);
        i++;
    }

    SDL_UnlockTexture(p_texture);

    SDL_RenderCopy(p_renderer, p_texture, NULL, NULL);
    SDL_RenderPresent(p_renderer);
}

int32_t sdl_help_screen_buffer_w() {
    return p_screen_buffer->w;
}

int32_t sdl_help_screen_buffer_h() {
    return p_screen_buffer->h;
}

void sdl_help_draw_vertical_line(int32_t x, int32_t start, int32_t end, uint8_t color) {
    uint8_t *pixels = (uint8_t*)p_screen_buffer->pixels;
    for (int32_t y = start; y <= end; y++) {
        pixels[x + y * p_screen_buffer->pitch] = color;
    }
}