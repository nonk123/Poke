#pragma once

#include <S_fixed.h>

#define XY(_x, _y) ((Vec2){.x = FxFrom(_x), .y = FxFrom(_y)})

typedef struct {
	Fixed x, y;
} Vec2;

Vec2 v2add(Vec2 a, Vec2 b);
Vec2 v2scale(Vec2 v, Fixed s);
