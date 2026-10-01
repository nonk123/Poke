#include "body.h"
#include "game.h"

static const Vec2 GRAVITY = XY(0.0, -9.8);

static void resolve_collision(Body* self, Body* other) {}

void simulate(Body* self, World* world) {
	// self->linvel = v2add(self->linvel, v2scale(GRAVITY, TIMESTEP));
	self->translation = v2add(self->translation, v2scale(self->linvel, TIMESTEP));

	for (size_t i = 0; i < TinyDLength(world->bodies); i++) {
		if (world->bodies + i != self) {
			Body* other = &world->bodies[i];
			resolve_collision(self, other);
		}
	}
}

void free_body(Body* self) {
	free_grid(&self->shape);
}
