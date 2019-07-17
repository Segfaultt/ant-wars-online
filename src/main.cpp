#include "common.h"
#include "entity.h"
#include "network.h"
#include "sdl.h"
#include "texture_wrapper.h"
#include "timer.h"

#include <bitset>
#include <chrono>
#include <mutex>
#include <thread>

SDL_Texture *get_map(const char *map) {}
int visual_entity_index;

void game_loop(SDL_Renderer *renderer, SDL_Window *window, int sockfd_s,
               sockaddr_in sock_addr_s) {
      //---=== Variables ===---
      bool quit = false;

      timer frame_cap;
      const int TICKS_PER_FRAME = 1000 / 60; // cap framerate to 60FPS

      SDL_Event e;
      int c_x = 0, c_y = 0;
      float c_z = 1;

      //---=== Preliminary Code ===---
      uint8_t buff[BUFFER_SIZE];
      do {
            std::memset(buff, 0, BUFFER_SIZE);

            std::cout << "waiting for index set packet" << std::endl;
            simple_receive(sockfd_s, sock_addr_s, buff);
      } while (buff[0] != 2);
      {
            int a = 1;
            visual_entity_index = uint8_t_to_uint32(buff, a);
            std::cout << "index set to " << visual_entity_index << std::endl;
      }

      //---=== Map Texture ===---
      do {
            std::memset(buff, 0, BUFFER_SIZE);
            std::cout << "waiting for map location packet" << std::endl;
            simple_receive(sockfd_s, sock_addr_s, buff);
      } while (buff[0] != 4);
      std::cout << "map location recieved: " << &buff[1] << std::endl;

      char
          map[BUFFER_SIZE]; // 500 bytes is a small price to pay to never be too
                            // small
      memcpy(map, &buff[1], BUFFER_SIZE - 1);
      SDL_Texture *bg_texture = NULL;
      std::cout << map;
      bg_texture = get_map(map);

      // mutex and render list for communications from server
      std::mutex render_list_mtx;
      std::vector<visual_entity> render_list;

      init_entities(renderer); // preload visual entities
      // create thread to handle packets from server
      std::thread listener([&render_list, &render_list_mtx, &buff, BUFFER_SIZE,
                            quit, sockfd_s, &sock_addr_s]() {
            while (!quit) {
                  std::memset(buff, 0, BUFFER_SIZE);
                  simple_receive(sockfd_s, sock_addr_s, buff);
                  switch (buff[0]) {
                  case 3: // visual entities
                        int a = 1;
                        std::vector<visual_entity> new_render_list;
                        visual_entity tmp;
                        while (uint8_t_to_uint32(buff, a) != 0) {
                              a -= 4;
                              tmp.id = uint8_t_to_uint32(buff, a);
                              tmp.et = (entity_type)uint8_t_to_uint32(buff, a);
                              tmp.x = uint8_t_to_uint32(buff, a);
                              tmp.y = uint8_t_to_uint32(buff, a);
                              tmp.angle = uint8_t_to_uint32(buff, a);
                              tmp.health = uint8_t_to_uint32(buff, a);
                              tmp.stamina = uint8_t_to_uint32(buff, a);
                              new_render_list.push_back(tmp);
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

            // handle SDL events
            while (SDL_PollEvent(&e)) {
                  // filter by event type
                  switch (e.type) {
                  case SDL_QUIT:
                        quit = true;
                        break;
                  }
            }

            // handle non-event keypresses
            const Uint8 *current_key_states = SDL_GetKeyboardState(NULL);
            if (current_key_states[SDL_SCANCODE_SPACE]) {
                  c_z *= 1.02;
            }
            if (current_key_states[SDL_SCANCODE_LSHIFT]) {
                  c_z *= 0.98;
            }
            if (current_key_states[SDL_SCANCODE_W]) {
                  uint8_t tmp[2];
                  tmp[0] = 5;
                  tmp[1] = 0;
                  simple_send(sockfd_s, sock_addr_s, tmp);
            } else if (current_key_states[SDL_SCANCODE_S]) {
                  uint8_t tmp[2];
                  tmp[0] = 5;
                  tmp[1] = 1;
                  simple_send(sockfd_s, sock_addr_s, tmp);
            }
            if (current_key_states[SDL_SCANCODE_D]) {
                  uint8_t tmp[2];
                  tmp[0] = 5;
                  tmp[1] = 2;
                  simple_send(sockfd_s, sock_addr_s, tmp);
            }
            if (current_key_states[SDL_SCANCODE_A]) {
                  uint8_t tmp[2];
                  tmp[0] = 5;
                  tmp[1] = 3;
                  simple_send(sockfd_s, sock_addr_s, tmp);
            }

            // make camera follow
            c_x += (render_list[visual_entity_index].x - c_x) / 40 ;
            c_y += (render_list[visual_entity_index].y - c_y) / 60 ;

            // render visual entities
            render_list_mtx.lock();
            for (visual_entity ve : render_list) {
                  // camera translations
                  double rx = c_x - ve.x;
                  int scrn_x = rx * c_z;
                  double ry = c_y - ve.y;
                  int scrn_y = ry * c_z;

                  SDL_Rect render_quad = {
                      scrn_x - entity_width[ve.et] * c_z / 2 + screen_width / 2,
                      scrn_y + screen_height / 2 - entity_height[ve.et] * c_z,
                      entity_width[ve.et] * c_z, entity_height[ve.et] * c_z};
                  SDL_RenderCopyEx(renderer, entity_texture[ve.et], NULL,
                                   &render_quad, (double)ve.angle + 270, NULL,
                                   SDL_FLIP_NONE);
            }
            render_list_mtx.unlock();

            // cap framerate
            if (TICKS_PER_FRAME > frame_cap.get_time()) {
                  SDL_Delay(TICKS_PER_FRAME - frame_cap.get_time());
            }
            SDL_RenderPresent(renderer);
      }
}

void menu_loop(SDL_Renderer *renderer, SDL_Window *window) {
      // set up style
      texture_wrapper title;
      title.load_text("Ant Wars Online", {0x0, 0x0, 0x0}, FONT, 40, renderer);

      // networking variables
      int sockfd_s;
      sockaddr_in sock_addr_s;

      // actual menu loop
      bool quit = false, joined = false;
      SDL_Event event;
      char buffer[15];
      srand(clock());
      int etype = rand() % (int)N_ENTITIES;
      std::memset(buffer, 0, 15);
      texture_wrapper buffer_texture, type_texture;
      buffer_texture.load_text("Enter IP address", {0x0, 0x0, 0x0}, FONT, 24,
                               renderer);
      type_texture.load_text("< " + entity_name[etype] + " >", {0x0, 0x0, 0x0},
                             FONT, 24, renderer);

      SDL_StartTextInput();
      while (!quit && !joined) {
            // handle user input
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
                                    buffer_texture.load_text(
                                        "Enter IP address", {0x0, 0x0, 0x0},
                                        FONT, 24, renderer);
                              else
                                    buffer_texture.load_text(
                                        buffer, {0x0, 0x0, 0x0}, FONT, 24,
                                        renderer);

                        } else if (event.key.keysym.sym == SDLK_RIGHT) {
                              etype = (++etype) % N_ENTITIES;

                              type_texture.load_text(
                                  "< " + entity_name[etype] + " >",
                                  {0x0, 0x0, 0x0}, FONT, 24, renderer);
                        } else if (event.key.keysym.sym == SDLK_LEFT) {
                              etype = (--etype + N_ENTITIES) % N_ENTITIES;

                              type_texture.load_text(
                                  "< " + entity_name[etype] + " >",
                                  {0x0, 0x0, 0x0}, FONT, 24, renderer);

                        } else if (event.key.keysym.sym == SDLK_RETURN) {
                              SDL_StopTextInput();
                              sender_init(sockfd_s, sock_addr_s, buffer);
                              uint8_t tmp[2];
                              tmp[0] = 1;
                              tmp[1] = (uint8_t)etype;
                              simple_send(sockfd_s, sock_addr_s, tmp);
                              joined = true;
                        }
                        break;
                  case SDL_TEXTINPUT:
                        if (strlen(buffer) < 15) {
                              strcat(buffer, event.text.text);
                              buffer_texture.load_text(buffer, {0x0, 0x0, 0x0},
                                                       FONT, 24, renderer);
                        }
                        break;
                  }
            }

            // render screen
            SDL_SetRenderDrawColor(renderer, 0xb0, 0x80, 0x99, 0xFF);
            SDL_RenderClear(renderer);

            // modulate and render title
            title.render_from_centre(screen_width / 2, screen_height / 3,
                                     renderer);
            type_texture.render_from_centre(screen_width / 2,
                                            screen_height * 2 / 3, renderer);

            // render foreground
            buffer_texture.render_from_centre(
                screen_width / 2, screen_height * 2 / 3 + 50, renderer);

            SDL_RenderPresent(renderer);
      }

      if (!quit)
            game_loop(renderer, window, sockfd_s, sock_addr_s);
}

int main(int argc, char *argv[]) {
      SDL_Renderer *renderer = NULL;
      SDL_Window *window = NULL;

      if (SDL_init(renderer, window) < 0) { // set up SDL2, renderer and window
            std::cout << "Could not initialise SDL2 fully" << std::endl;
            exit(-1);
      }

      menu_loop(renderer, window);

      SDL_close(renderer, window);

      return 0;
}
