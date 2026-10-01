#pragma once

#include <SDL3/SDL_video.h>

typedef struct {
	uint8_t r, g, b;
} Palette;

typedef uint8_t PaletteIndex;
enum {
	PLT_DUNG,
	PLT_TUNG,
	PLT_COUNT,
};

extern Palette g_palette[256];
