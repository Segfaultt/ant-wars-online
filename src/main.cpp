#include "common.h"
#include "sdl.h"
#include "texture_wrapper.h"
#include "network.h"

void menu_loop (SDL_Renderer* renderer, SDL_Window* window)
{
	//randomly pick faction
	srand(clock());
	int theme = rand()%3;

	//set up style
	texture_wrapper title;
	title.load_text("Ant Wars Colonial", COLOR_TITLE[theme], FONT[theme], SIZE_TITLE[theme], renderer);

	//networking variables
	int sockfd;
	sockaddr_in sock_addr;

	//actual menu loop
	bool quit = false, joined = false;
	SDL_Event event;
	char buffer[15];
	std::memset(buffer, 0, 15);
	texture_wrapper buffer_texture, faction_texture;
	buffer_texture.load_text("Enter IP address", COLOR_FG[theme], FONT[theme], SIZE_FG[theme], renderer);
	faction_texture.load_text("< " + NAME[theme] + " >", COLOR_FG[theme], FONT[theme], SIZE_FG[theme], renderer);
	SDL_StartTextInput();
	while (!quit && !joined) {
		//handle user input
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				quit = true;
				break;

			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_BACKSPACE) {
					if (strlen(buffer) > 0)
						*(buffer + strlen(buffer) - 1) = 0;
					if (strlen(buffer) == 0)
						buffer_texture.load_text("Enter IP address", COLOR_FG[theme], FONT[theme], SIZE_FG[theme], renderer);
					else
						buffer_texture.load_text(buffer, COLOR_FG[theme], FONT[theme], SIZE_FG[theme], renderer);
				} else if (event.key.keysym.sym == SDLK_LEFT) {
					theme = (2 + theme)%3;//-1%3 gives -1 which is undesirable so I increment by 2 instead of decrementing

					title.load_text("Ant Wars Colonial", COLOR_TITLE[theme], FONT[theme], SIZE_TITLE[theme], renderer);
					faction_texture.load_text("< " + NAME[theme] + " >", COLOR_FG[theme], FONT[theme], SIZE_FG[theme], renderer);

					if (strlen(buffer) == 0)
						buffer_texture.load_text("Enter IP address", COLOR_FG[theme], FONT[theme], SIZE_FG[theme], renderer);
					else
						buffer_texture.load_text(buffer, COLOR_FG[theme], FONT[theme], SIZE_FG[theme], renderer);
				} else if (event.key.keysym.sym == SDLK_RIGHT) {
					theme = (theme + 1)%3;
					title.load_text("Ant Wars Colonial", COLOR_TITLE[theme], FONT[theme], SIZE_TITLE[theme], renderer);
					faction_texture.load_text("< " + NAME[theme] + " >", COLOR_FG[theme], FONT[theme], SIZE_FG[theme], renderer);

					if (strlen(buffer) == 0)
						buffer_texture.load_text("Enter IP address", COLOR_FG[theme], FONT[theme], SIZE_FG[theme], renderer);
					else
						buffer_texture.load_text(buffer, COLOR_FG[theme], FONT[theme], SIZE_FG[theme], renderer);
				} else if (event.key.keysym.sym == SDLK_RETURN) {
					SDL_StopTextInput();
					sender_init(sockfd, sock_addr, buffer);
					simple_send(sockfd, sock_addr, "Join");
				}
				break;
			case SDL_TEXTINPUT:
				if (strlen(buffer) < 15) {
					strcat(buffer, event.text.text);
					buffer_texture.load_text(buffer, COLOR_FG[theme], FONT[theme], SIZE_FG[theme], renderer);
				}
				break;
			}
		}

		//render screen
		SDL_SetRenderDrawColor(renderer, COLOR_BG[theme].r, COLOR_BG[theme].g, COLOR_BG[theme].b, 0xFF);
		SDL_RenderClear(renderer);

		//modulate and render title
		title.render_from_centre(screen_width/2, screen_height/3, renderer);

		//render foreground
		buffer_texture.render_from_centre(screen_width/2, screen_height*2/3 + 50, renderer);
		faction_texture.render_from_centre(screen_width/2, screen_height*2/3, renderer);

		SDL_RenderPresent(renderer);
	}
}

int main (int argc, char* argv[])
{
	SDL_Renderer* renderer = NULL;
	SDL_Window* window = NULL;

	if (SDL_init(renderer, window) < 0) {//set up SDL2, renderer and window
		std::cout << "Could not initialise SDL2 fully" << std::endl;
		exit(-1);
	}

	menu_loop(renderer, window);

	SDL_close(renderer, window);

	return 0;
}
