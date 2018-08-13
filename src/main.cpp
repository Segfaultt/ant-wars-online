#include "common.h"
#include "sdl.h"
#include "texture_wrapper.h"
#include "network.h"
#include "timer.h"
#include "entity.h"

#include <bitset>
#include <mutex>
#include <thread>

void game_loop (SDL_Renderer* renderer, SDL_Window* window, int sockfd_s, sockaddr_in sock_addr_s)
{
	//---=== Variables ===---
	bool quit = false;

	timer frame_cap;
	const int TICKS_PER_FRAME = 1000/60;//cap framerate to 60FPS

	SDL_Event e;
	int c_x, c_y;
	float c_z;

	//---=== Preliminary Code ===---
	uint8_t buff[BUFFER_SIZE];
	do {
		std::memset(buff, 0, BUFFER_SIZE);
		std::cout << "waiting for camera set packet" << std::endl;
		simple_receive(sockfd_s, sock_addr_s, buff);
	} while (buff[0] != 2);
	{
		int a = 1;
		c_x = uint8_t_to_uint32(buff, a);
		c_y = uint8_t_to_uint32(buff, a);
		c_z = uint8_t_to_uint32(buff, a)/1024.0;
		std::cout << "camera set to: (" << c_x << ", " << c_y << ") with zoom: " << c_z << std::endl;
	}

	//mutex and render list for communications from server
	std::mutex render_list_mtx;
	std::vector<visual_entity> render_list;

	init_entities(renderer);
	std::thread listener([&render_list, &render_list_mtx, &buff, BUFFER_SIZE, quit, sockfd_s, &sock_addr_s](){
				while (!quit) {
					std::memset(buff, 0, BUFFER_SIZE);
					simple_receive(sockfd_s, sock_addr_s, buff);
					switch (buff[0]) {
						case 3://visual entities
						int a = 1;
						std::vector<visual_entity> new_render_list;
						visual_entity tmp;
						tmp.id = uint8_t_to_uint32(buff, a);
						tmp.et = (entity_type)uint8_t_to_uint32(buff, a);
						tmp.x = uint8_t_to_uint32(buff, a);
						tmp.y = uint8_t_to_uint32(buff, a);
						tmp.angle = uint8_t_to_uint32(buff, a)/1024.0;
						tmp.health = uint8_t_to_uint32(buff, a);
						tmp.stamina = uint8_t_to_uint32(buff, a);
						while (tmp.id != 0) {
							new_render_list.push_back(tmp);
							tmp.id = uint8_t_to_uint32(buff, a);
							tmp.et = (entity_type)uint8_t_to_uint32(buff, a);
							tmp.x = uint8_t_to_uint32(buff, a);
							tmp.y = uint8_t_to_uint32(buff, a);
							tmp.angle = uint8_t_to_uint32(buff, a)/1024.0;
							tmp.health = uint8_t_to_uint32(buff, a);
							tmp.stamina = uint8_t_to_uint32(buff, a);
						}
						render_list_mtx.lock();
						render_list.clear();
						render_list = new_render_list;
						render_list_mtx.unlock();
					}
				}
			});

	//---=== Game Loop ===---
	while (!quit) {
		frame_cap.start();
		SDL_RenderClear(renderer);

		//handle SDL events
		while (SDL_PollEvent(&e)) {
			//filter by event type
			switch (e.type) {
				case SDL_QUIT:
					quit = true;
					break;
			}
		}

		//handle non-event keypresses
		const Uint8* current_key_states = SDL_GetKeyboardState(NULL);
		if (current_key_states[SDL_SCANCODE_SPACE]) {
			c_z *= 1.02;
		}
		if (current_key_states[SDL_SCANCODE_LSHIFT]) {
			c_z *= 0.98;
		}

		//render visual entities
		render_list_mtx.lock();
		for (visual_entity ve: render_list) {
			//camera translations
			int scrn_x, scrn_y;
			int rx = c_x - ve.x;
			scrn_x = rx*c_z;
			int ry = c_y - ve.y;
			scrn_y = ry/c_z;

			SDL_Rect render_quad = {scrn_x, scrn_y, entity_width[ve.et]*c_z, entity_height[ve.et]*c_z};
			SDL_RenderCopyEx(renderer, entity_texture[ve.et], NULL, &render_quad, ve.angle, NULL, SDL_FLIP_NONE);
		}
		render_list_mtx.unlock();

		//cap framerate
		if (TICKS_PER_FRAME > frame_cap.get_time()) {
			SDL_Delay(TICKS_PER_FRAME - frame_cap.get_time());
		}
		SDL_RenderPresent(renderer);
	}
}

void menu_loop (SDL_Renderer* renderer, SDL_Window* window)
{
	//randomly pick faction
	srand(clock());
	int theme = rand()%3;

	//set up style
	texture_wrapper title;
	title.load_text("Ant Wars Colonial", COLOR_TITLE[theme], FONT[theme], SIZE_TITLE[theme], renderer);

	//networking variables
	int sockfd_s;
	sockaddr_in sock_addr_s;

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
						sender_init(sockfd_s, sock_addr_s, buffer);
						uint8_t tmp[2];
						tmp[0] = 1;
						tmp[1] = (uint8_t)theme;
						simple_send(sockfd_s, sock_addr_s, tmp);
						joined = true;
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

	if (!quit)
		game_loop(renderer, window, sockfd_s, sock_addr_s);
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
