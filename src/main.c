#define SDL_MAIN_USE_CALLBACKS
#define S_TRUCTURES_IMPLEMENTATION
#define FIX_IMPLEMENTATION

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <NutBlast.h>

#include "body.h"
#include "cmake.h"
#include "sdl.h"

SDL_Window* g_window = NULL;
SDL_Renderer* g_renderer = NULL;
static World world = {0};

static const int SCALE = 24;

static SDL_AppResult die() {
	SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Shit! %s", SDL_GetError());
	return SDL_APP_FAILURE;
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
	(void)appstate, (void)argc, (void)argv;

	NutBlast_Init((NutBlast_InitOptions){.game_id = GAME_NAME});

	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS))
		return die();

	const SDL_WindowFlags fags = SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE;

	if (!SDL_CreateWindowAndRenderer(GAME_NAME, 800, 600, fags, &g_window, &g_renderer))
		return die();

	// TODO: sync ticks to tickrate exactly.
	SDL_SetRenderVSync(g_renderer, 1);

	world.bodies = MakeTinyD(Body);

	Body body = {0};

	body.translation = XY(16, 48);

	grid_put(&body.shape, GRID_XY(0, 0), PLT_TUNG);
	grid_put(&body.shape, GRID_XY(1, 0), PLT_DUNG);
	grid_put(&body.shape, GRID_XY(0, 1), PLT_DUNG);
	grid_put(&body.shape, GRID_XY(1, 1), PLT_TUNG);

	world.bodies = TinyDPush(world.bodies, &body);

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
	(void)appstate;

	switch (event->type) {
	case SDL_EVENT_QUIT:
		return SDL_APP_SUCCESS;
	case SDL_EVENT_KEY_DOWN:
		if (event->key.key == SDLK_ESCAPE)
			return SDL_APP_SUCCESS;
	default:
		break;
	}

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
	(void)appstate;

	SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(g_renderer);

	GridPoint camera_pos = GRID_XY(0, 0);

	SDL_Rect sdl_viewport = {0};
	SDL_GetRenderViewport(g_renderer, &sdl_viewport);

	AABB viewport = {camera_pos};

	viewport.max = viewport.min;
	viewport.max.x += sdl_viewport.w / SCALE;
	viewport.max.y += sdl_viewport.h / SCALE;

	for (size_t i = 0; i < TinyDLength(world.bodies); i++) {
		Body* body = &world.bodies[i];
		simulate(body, &world);

		AABB aabb = grid_aabb(&body->shape);

		if (aabb.min.x < viewport.min.x || aabb.min.x > viewport.max.x)
			continue;

		if (aabb.min.y < viewport.min.y || aabb.min.y > viewport.max.y)
			continue;

		// TODO: project the rotated & translated shape onto the main grid
		TINY_MAP_FOREACH (&body->shape.cells, it) {
			Palette plt = g_palette[*(PaletteIndex*)it.data];
			SDL_SetRenderDrawColor(g_renderer, plt.r, plt.g, plt.b, SDL_ALPHA_OPAQUE);

			GridPoint point = uncrunch_point(it.bucket->hash);
			point.x += camera_pos.x + Fx2Int(body->translation.x);
			point.y += camera_pos.y + Fx2Int(body->translation.y);

			SDL_FRect rect = {0};
			rect.x = (float)point.x * (float)SCALE;
			rect.y = (float)sdl_viewport.h - (float)(point.y + 1) * (float)SCALE;
			rect.w = rect.h = (float)SCALE;

			SDL_RenderFillRect(g_renderer, &rect);
		}
	}

	SDL_RenderPresent(g_renderer);

	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
	(void)appstate, (void)result;

	for (size_t i = 0; i < TinyDLength(world.bodies); i++)
		free_body(&world.bodies[i]);

	NutBlast_Cleanup();
}
