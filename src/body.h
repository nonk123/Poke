#pragma once

#include "grid.h"
#include "vec2.h"

typedef struct {
	Grid shape;
	Vec2 translation, linvel;
	Fixed angle, angvel;
} Body;

typedef struct {
	Body* bodies;
} World;

void simulate(Body*, World*);
void free_body(Body*);
