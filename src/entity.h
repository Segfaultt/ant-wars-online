#pragma once
#include "sdl.h"

#include <iostream>

enum entity_type {
	NIPPER,
	MOONBOY,
	TESLA,
	TRAP,
	WORKER,
	BUILDER,
	FLYING,
	HIPSTER,
	LASER,
	N_ENTITIES
};

struct visual_entity {
	int id;
	entity_type et;
	int  x, y, angle;
	int  health, stamina;
	friend std::ostream& operator<< (std::ostream &os, visual_entity &ve);
};

extern const char *entity_texture_file[N_ENTITIES];

extern SDL_Texture *entity_texture[N_ENTITIES];

extern unsigned int entity_width[N_ENTITIES];
extern unsigned int entity_height[N_ENTITIES];

void init_entities(SDL_Renderer *renderer);

