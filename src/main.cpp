#include <SDL3/SDL.h>
#include <sys/mman.h>
#include <stdint.h>

#define internal static
#define global_variable static
#define local_persist static

#define PIXEL_SIZE 4

global_variable SDL_Window *window;
global_variable SDL_Renderer *renderer;
global_variable SDL_Texture *texture;
global_variable int done = 0;
global_variable int texture_width;
global_variable int texture_height;
global_variable void *pixels;

typedef uint8_t  uint8; 
typedef uint16_t uint16;
typedef uint32_t uint32; 
typedef uint64_t uint64; 

internal void weird_gradient(int offset_x, int offset_y)
{
    uint32 *current_pixel = (uint32 *)pixels;

    for (int y = 0; y < texture_height; y++) {
        for (int x = 0; x < texture_width; x++) {
            uint8 blue  = x + offset_x;
            uint8 green = y + offset_y;

            // ARGB8888: 0xAARRGGBB
            *current_pixel++ = (0xFF << 24) | (green << 8) | blue;
        }
    }
}


internal void resize_texture(int width, int height)
{

	if(pixels) {
		munmap(pixels, texture_width * texture_height * PIXEL_SIZE);
	}

	if(texture) {
		SDL_DestroyTexture(texture);
	}

	texture_width = width;
	texture_height = height;

	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, texture_width, texture_height);	

	size_t num_of_pixels = texture_width * texture_height * PIXEL_SIZE;
	pixels = mmap(NULL, num_of_pixels, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

}

internal void render_window()
{

	SDL_Rect client_rect = {0, 0, texture_width, texture_height};
	SDL_UpdateTexture(texture, &client_rect, pixels, texture_width * PIXEL_SIZE);
	SDL_RenderTexture(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
	SDL_Delay(16);
}


internal void handle_events(SDL_Event *event)
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

		resize_texture(width, height);

		break;
	default:
		SDL_Log("Unhandled Event: %d", event->type);
	}

}

internal void initialise_texture()
{

	int width, height;
	SDL_GetWindowSize(window, &width, &height);
	resize_texture(width, height);

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

	initialise_texture();
	
	int offset_x = 0;
	int offset_y = 0;
	
	while(!done) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			handle_events(&event);
		}
		
		weird_gradient(offset_x++, offset_y++);
		render_window();

	}

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyTexture(texture);
	SDL_Quit();

	return 0;

}

