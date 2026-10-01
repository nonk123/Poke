#define SDL_MAIN_USE_CALLBACKS
#define S_TRUCTURES_IMPLEMENTATION
#define FIX_IMPLEMENTATION

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <NutBlast.h>

#include "body.h"
#include "cmake.h"
#include "game.h"
#include "sdl.h"
#include "viewport.h"

SDL_Window* g_window = NULL;
SDL_Renderer* g_renderer = NULL;
static World world = {0};

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
	const int hw = 7, hh = 3;

	for (int x = -hw; x <= hw; x++) {
		for (int y = -hh; y <= hh; y++) {
			PaletteIndex idx = PLT_DUNG;

			if (SDL_abs(x) == hw || SDL_abs(y) == hh)
				idx = PLT_TUNG;

			grid_put(&body.shape, GRID_XY(x, y), idx);
		}
	}

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

	const bool* kb = SDL_GetKeyboardState(NULL);
	g_camera.position.x += (int)(kb[SDL_SCANCODE_D]) - (int)(kb[SDL_SCANCODE_A]);
	g_camera.position.y += (int)(kb[SDL_SCANCODE_W]) - (int)(kb[SDL_SCANCODE_S]);

	SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(g_renderer);

	for (size_t i = 0; i < TinyDLength(world.bodies); i++) {
		Body* body = &world.bodies[i];
		simulate(body, &world);

		Grid rotated = grid_rotate(&body->shape, body->angle);
		body->angle = Fadd(body->angle, Fmul(Fx2Pi, TIMESTEP));

		AABB aabb = grid_aabb(&rotated);

		aabb.min.x += Fx2Int(body->translation.x);
		aabb.min.y += Fx2Int(body->translation.y);

		aabb.max.x += Fx2Int(body->translation.x);
		aabb.max.y += Fx2Int(body->translation.y);

		if (aabb.max.x < viewport().min.x || aabb.min.x > viewport().max.x)
			goto next;

		if (aabb.max.y < viewport().min.y || aabb.min.y > viewport().max.y)
			goto next;

		TINY_MAP_FOREACH (&rotated.cells, it) {
			Palette plt = g_palette[*(PaletteIndex*)it.data];
			SDL_SetRenderDrawColor(g_renderer, plt.r, plt.g, plt.b, SDL_ALPHA_OPAQUE);

			GridPoint point = uncrunch_point(it.bucket->hash);
			point.x += Fx2Int(body->translation.x);
			point.y += Fx2Int(body->translation.y);

			SDL_FRect rect = {0};
			rect.x = (float)(point.x - viewport().min.x) * (float)g_camera.zoom;
			rect.y = (float)screen_viewport().max.y;
			rect.y -= (float)(point.y + 1 - viewport().min.y) * (float)g_camera.zoom;
			rect.w = rect.h = (float)g_camera.zoom;

			SDL_RenderFillRect(g_renderer, &rect);
		}

	next:
		free_grid(&rotated);
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
