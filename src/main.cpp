#include <SDL3/SDL.h>

int main(void)
{
	SDL_Window *window;
	SDL_Renderer *renderer;
	bool done = false;

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_Log("SDL_Init failed: %s", SDL_GetError());
		return 1;
	}

	window = SDL_CreateWindow("Hello From SDL3", 800, 600, 0);

	if (!window) {
		SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
		return 1;
	}

	renderer = SDL_CreateRenderer(window, 0);

	if (!renderer) {
		SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
		return 1;
	}

	while(!done) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_EVENT_QUIT:
				SDL_Log("Event: Quit");
				done = true;
				break;
			default:
				SDL_Log("Unhandled Event: %d", event.type);
			}
		}

		static bool is_white = true;

		if (is_white) {
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			is_white = false;
		} else {
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			is_white = true;
		}

		SDL_RenderClear(renderer);
		SDL_RenderPresent(renderer);

		SDL_Delay(100);

	}

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	return 0;
}
