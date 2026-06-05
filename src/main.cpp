#include <SDL3/SDL.h>

int main(void)
{
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *texture;
	bool done = false;

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_Log("SDL_Init failed: %s", SDL_GetError());
		return 1;
	}

	window = SDL_CreateWindow("Hello From SDL3", 800, 600, SDL_WINDOW_RESIZABLE);

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
			case SDL_EVENT_WINDOW_RESIZED:
				int width, height;

				SDL_GetWindowSize(window, &width, &height);
				SDL_Log("Event: Window Resized to @%dx%d", width, height);

				if(texture) {
					SDL_DestroyTexture(texture);
				}

				texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);
				break;
			default:
				SDL_Log("Unhandled Event: %d", event.type);
			}
		}

		SDL_RenderTexture(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	return 0;
}
