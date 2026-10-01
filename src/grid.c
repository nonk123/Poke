#include "grid.h"

typedef union {
	uint32_t u32;
	GridCoord grid;
} PUN;

TinyHash crunch_point(GridPoint p) {
	PUN x, y;

	x.grid = p.x;
	y.grid = p.y;

	return (uint64_t)x.u32 | ((uint64_t)y.u32 << 32);
}

GridPoint uncrunch_point(TinyHash h) {
	PUN x, y;

	x.u32 = h & 0xFFFFFFFF;
	y.u32 = h >> 32;

	return GRID_XY(x.grid, y.grid);
}

PaletteIndex* grid_at(Grid* self, GridPoint p) {
	return (PaletteIndex*)TinyMapGet(&self->cells, crunch_point(p));
}

PaletteIndex* grid_put(Grid* self, GridPoint p, PaletteIndex cell) {
	return TinyMapPut(&self->cells, crunch_point(p), &cell, sizeof(cell))->data;
}

void free_grid(Grid* self) {
	FreeTinyMap(&self->cells);
}

AABB grid_aabb(Grid* self) {
	AABB aabb = {0};

	aabb.max.x = aabb.max.y = INT32_MIN;
	aabb.min.x = aabb.min.y = INT32_MAX;

	TINY_MAP_FOREACH (&self->cells, it) {
		GridPoint p = uncrunch_point(it.bucket->hash);

		aabb.min.x = SDL_min(aabb.min.x, p.x);
		aabb.min.y = SDL_min(aabb.min.y, p.y);

		aabb.max.x = SDL_max(aabb.max.x, p.x);
		aabb.max.y = SDL_max(aabb.max.y, p.y);
	}

	return aabb;
}

static Grid shear_x(Grid* self, Fixed k) {
	Grid out = {0};

	TINY_MAP_FOREACH (&self->cells, it) {
		GridPoint p = uncrunch_point(it.bucket->hash);
		p.x += (GridCoord)Fx2Int(Fmul(k, FxFrom(p.y)));
		grid_put(&out, p, *(PaletteIndex*)it.data);
	}

	return out;
}

static Grid shear_y(Grid* self, Fixed k) {
	Grid out = {0};

	TINY_MAP_FOREACH (&self->cells, it) {
		GridPoint p = uncrunch_point(it.bucket->hash);
		p.y += (GridCoord)Fx2Int(Fmul(k, FxFrom(p.x)));
		grid_put(&out, p, *(PaletteIndex*)it.data);
	}

	return out;
}

Grid grid_rotate(Grid* self, Fixed angle) {
	if (Fabs(angle) == FxPi) {
		Grid out = {0};

		TINY_MAP_FOREACH (&self->cells, it) {
			GridPoint p = uncrunch_point(it.bucket->hash);
			p.x = -p.x, p.y = -p.y;
			grid_put(&out, p, *(PaletteIndex*)it.data);
		}

		return out;
	} else {
		const Fixed t = Ftan(Fmul(angle, FxFrom(0.5)));
		const Fixed s = Fsin(angle);

		Grid g1 = shear_x(self, -t);
		Grid g2 = shear_y(&g1, s);
		free_grid(&g1);

		Grid out = shear_x(&g2, -t);
		free_grid(&g2);

		return out;
	}
}
