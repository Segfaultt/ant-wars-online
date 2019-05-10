/*
  contains code for entities which include all non-ui, non-background things
  such as ants and structures
*/
#include "entity.h"

const char *entity_texture_file[N_ENTITIES] = {
    "res/ant-nipper.png",   "res/ant-moonboy.png", "res/ant-tesla.png",
    "res/ant-trap-jaw.png", "res/ant-worker.png",  "res/ant-builder.png",
    "res/ant-flying.png",   "res/ant-hipster.png", "res/ant-laser.png"};

SDL_Texture *entity_texture[N_ENTITIES];

unsigned int entity_width[N_ENTITIES];
unsigned int entity_height[N_ENTITIES];

void init_entities(SDL_Renderer *renderer) {
      for (int i = 0; i < N_ENTITIES; i++) {
            entity_texture[i] = NULL;
      }

      for (int i = 0; i < N_ENTITIES; i++) {
            SDL_Surface *surf = IMG_Load(entity_texture_file[i]);
            if (surf == NULL) {
                  std::cout << "could not load image: " << SDL_GetError()
                            << std::endl;
            } else {
                  entity_texture[i] =
                      SDL_CreateTextureFromSurface(renderer, surf);
                  if (entity_texture[i] == NULL) {
                        std::cout
                            << "could not create texture: " << SDL_GetError()
                            << std::endl;
                  } else {
                        entity_width[i] = surf->w;
                        entity_height[i] = surf->h;
                  }
                  SDL_FreeSurface(surf);
            }
      }
}

std::ostream &operator<<(std::ostream &os, visual_entity &ve) {
      os << "id: " << ve.id << std::endl;
      os << "et: " << ve.et << std::endl;
      os << "(x, y): (" << ve.x << ", " << ve.y << ")" << std::endl;
      os << "angle: " << ve.angle << std::endl;
      os << "health: " << ve.health << std::endl;
      os << "stamina: " << ve.stamina << std::endl << std::endl;

      return os;
}
