#pragma once

#include <SDL3/SDL_stdinc.h>

#include <S_fixed.h>
#include <S_tructures.h>

#include "palette.h"

#define GRID_XY(_x, _y) ((GridPoint){.x = (_x), .y = (_y)})

typedef int32_t GridCoord;

typedef struct {
	GridCoord x, y;
} GridPoint;

typedef struct {
	TinyMap cells;
} Grid;

typedef struct {
	GridPoint min, max;
} AABB;

PaletteIndex *grid_at(Grid*, GridPoint), *grid_put(Grid*, GridPoint, PaletteIndex);
AABB grid_aabb(Grid*);
void free_grid(Grid*);

TinyHash crunch_point(GridPoint);
GridPoint uncrunch_point(TinyHash);

Grid grid_rotate(Grid*, Fixed angle);

AABB viewport();
