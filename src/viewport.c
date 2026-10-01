#include "sdl.h"
#include "viewport.h"

struct Camera g_camera = {
	.position = GRID_XY(0, 0),
	.zoom = 24,
};

AABB screen_viewport() {
	SDL_Rect rect = {0};
	SDL_GetRenderViewport(g_renderer, &rect);

	AABB out = {0};

	out.min.x = out.max.x = rect.x;
	out.min.y = out.max.y = rect.y;

	out.max.x += rect.w;
	out.max.y += rect.h;

	return out;
}

AABB viewport() {
	AABB out = screen_viewport();

	out.min.x /= g_camera.zoom;
	out.min.y /= g_camera.zoom;

	out.min.x += g_camera.position.x;
	out.min.y += g_camera.position.y;

	out.max.x /= g_camera.zoom;
	out.max.y /= g_camera.zoom;

	out.max.x += g_camera.position.x;
	out.max.y += g_camera.position.y;

	return out;
}
