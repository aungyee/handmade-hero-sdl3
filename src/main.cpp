#include <SDL3/SDL.h>
#include <sys/mman.h>
#include <stdint.h>

#define internal static
#define global_variable static
#define local_persist static

struct BackBuffer {
	SDL_Texture *texture;
	int width;
	int height;
	void *pixels;
	int pixel_size;
};

typedef uint8_t  uint8; 
typedef uint16_t uint16;
typedef uint32_t uint32; 
typedef uint64_t uint64; 

global_variable int done = 0;
global_variable SDL_Window *window;
global_variable SDL_Renderer *renderer;
global_variable BackBuffer backbuffer = { .texture = NULL, .width = 0, .height = 0, .pixels = 0, .pixel_size = 4 };

internal void weird_gradient(BackBuffer *backbuffer, int offset_x, int offset_y)
{
    uint32 *current_pixel = (uint32 *)backbuffer->pixels;

    for (int y = 0; y < backbuffer->height; y++) {
        for (int x = 0; x < backbuffer->width; x++) {
            uint8 blue  = x + offset_x;
            uint8 green = y + offset_y;

            // ARGB8888: 0xAARRGGBB
            *current_pixel++ = (0xFF << 24) | (green << 8) | blue;
        }
    }
}


internal void resize_texture(BackBuffer *backbuffer, int width, int height)
{

	if(backbuffer->pixels) {
		munmap(backbuffer->pixels, backbuffer->width * backbuffer->height * backbuffer->pixel_size);
	}

	if(backbuffer->texture) {
		SDL_DestroyTexture(backbuffer->texture);
	}

	backbuffer->width = width;
	backbuffer->height = height;

	backbuffer->texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, backbuffer->width, backbuffer->height);	

	size_t num_of_pixels = backbuffer->width * backbuffer->height * backbuffer->pixel_size;
	backbuffer->pixels = mmap(NULL, num_of_pixels, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

}

internal void render_window(BackBuffer *backbuffer)
{
	int width, height;	
	SDL_GetWindowSize(window, &width, &height);
	SDL_Rect client_rect = {0, 0, width, height};
	SDL_Rect buffer_rect = {0, 0, backbuffer->width, backbuffer->height};
	SDL_FRect client_frect, buffer_frect;
	SDL_RectToFRect(&client_rect, &client_frect);
	SDL_RectToFRect(&buffer_rect, &buffer_frect);
	SDL_UpdateTexture(backbuffer->texture, &client_rect, backbuffer->pixels, backbuffer->width * backbuffer->pixel_size);
	SDL_RenderTexture(renderer, backbuffer->texture, &buffer_frect, &client_frect);
	SDL_RenderPresent(renderer);
	SDL_Delay(16);
}


internal void handle_events(BackBuffer *backbuffer, SDL_Event *event)
{
	
	int width, height;

	switch (event->type) {
	case SDL_EVENT_QUIT:
		SDL_Log("Event: Quit");
		done = 1;
		break;
	case SDL_EVENT_WINDOW_RESIZED:
		SDL_GetWindowSize(window, &width, &height);
		SDL_Log("Event: Window Resized to @%dx%d", width, height);

		resize_texture(backbuffer, width, height);
		break;
	default:
		SDL_Log("Unhandled Event: %d", event->type);
	}

}

internal void initialise_texture(BackBuffer *backbuffer)
{

	int width, height;
	SDL_GetWindowSize(window, &width, &height);
	resize_texture(backbuffer, width, height);

}

int main(void)
{

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

	initialise_texture(&backbuffer);
	
	int offset_x = 0;
	int offset_y = 0;
	
	while(!done) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			handle_events(&backbuffer, &event);
		}
		
		weird_gradient(&backbuffer, offset_x++, offset_y++);
		render_window(&backbuffer);

	}

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyTexture(backbuffer.texture);
	SDL_Quit();

	return 0;

}

