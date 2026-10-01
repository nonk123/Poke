#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <NutBlast.h>

#include "cmake.h"
#include "sdl.h"

SDL_Window* g_window = NULL;
SDL_Renderer* g_renderer = NULL;

static SDL_AppResult die() {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Shit! %s", SDL_GetError());
    return SDL_APP_FAILURE;
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
    (void)appstate, (void)argc, (void)argv;

    NutBlast_Init((NutBlast_InitOptions){.game_id = GAME_NAME});

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS))
        return die();

    if (!SDL_CreateWindowAndRenderer(GAME_NAME, 800, 600, SDL_WINDOW_MAXIMIZED, &g_window, &g_renderer))
        return die();

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

    SDL_RenderPresent(g_renderer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    (void)appstate, (void)result;

    NutBlast_Cleanup();
}
