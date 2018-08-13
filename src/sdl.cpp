#include "common.h"
#include "entity.h"

int SDL_init (SDL_Renderer*& renderer, SDL_Window*& window)
{
	int return_value = 0;
	
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cerr << "Could not init SDL: " << SDL_GetError() << std::endl;
		return_value = -1;
	} else {
		window = SDL_CreateWindow("ANT WARS COLONIAL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height, SDL_WINDOW_SHOWN);
		if (window == NULL) {
			std::cerr << "Could not create window: " << SDL_GetError() << std::endl;
			return_value = -2;
		} 
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
		if (renderer == NULL) {
			std::cerr << "Could not create renderer: " << SDL_GetError() << std::endl;
			return_value = -3;
		}  //Initialize SDL_ttf
		if( TTF_Init() == -1 ) {
			std::cerr << "Could not initialise SDL_ttf: " << SDL_GetError() << std::endl;
			return_value = -4;
		}
	}

	return return_value;
}

void SDL_close (SDL_Renderer*& renderer, SDL_Window*& window)
{
	SDL_DestroyRenderer(renderer);
	renderer = NULL;
	SDL_DestroyWindow(window);
	window = NULL;
}
