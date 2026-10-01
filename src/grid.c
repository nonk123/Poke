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

static GridCoord fx_floor(Fixed v) {
	GridCoord i = FxToInt(v);
	if (FxFrom((double)i) > v)
		i--;
	return i;
}

static GridCoord fx_ceil(Fixed v) {
	GridCoord i = FxToInt(v);
	if (FxFrom((double)i) < v)
		i++;
	return i;
}

Grid grid_rotate(Grid* self, Fixed angle) {
	Grid out = {0};

	const Fixed c = Fcos(angle);
	const Fixed s = Fsin(angle);

	const AABB aabb = grid_aabb(self);

	const Fixed x0 = Fsub(FxFrom((double)aabb.min.x), FxHalf);
	const Fixed y0 = Fsub(FxFrom((double)aabb.min.y), FxHalf);
	const Fixed x1 = Fadd(FxFrom((double)aabb.max.x), FxHalf);
	const Fixed y1 = Fadd(FxFrom((double)aabb.max.y), FxHalf);

	const Fixed cx[4] = {x0, x1, x0, x1};
	const Fixed cy[4] = {y0, y0, y1, y1};

	GridCoord minx = 0, miny = 0, maxx = 0, maxy = 0;

	for (int i = 0; i < 4; i++) {
		const Fixed rx = Fsub(Fmul(c, cx[i]), Fmul(s, cy[i]));
		const Fixed ry = Fadd(Fmul(s, cx[i]), Fmul(c, cy[i]));

		const GridCoord ix = fx_floor(rx);
		const GridCoord iy = fx_floor(ry);
		const GridCoord ax = fx_ceil(rx);
		const GridCoord ay = fx_ceil(ry);

		if (i == 0) {
			minx = ix;
			maxx = ax;
			miny = iy;
			maxy = ay;
		} else {
			if (ix < minx)
				minx = ix;
			if (ax > maxx)
				maxx = ax;
			if (iy < miny)
				miny = iy;
			if (ay > maxy)
				maxy = ay;
		}
	}

	for (GridCoord dy = miny; dy <= maxy; dy++) {
		for (GridCoord dx = minx; dx <= maxx; dx++) {
			const Fixed fdx = FxFrom(dx);
			const Fixed fdy = FxFrom(dy);

			const Fixed sx = Fadd(Fmul(c, fdx), Fmul(s, fdy));
			const Fixed sy = Fadd(Fmul(-s, fdx), Fmul(c, fdy));

			PaletteIndex* v = grid_at(self, GRID_XY(FxToInt(sx), FxToInt(sy)));

			if (v)
				grid_put(&out, GRID_XY(dx, dy), *v);
		}
	}

	return out;
}
