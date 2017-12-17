#pragma once 

#include <iostream>
#include <ctime>
#include <chrono>
#include <math.h>
#include <vector>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

const unsigned int screen_width = 1366, screen_height = 768;

enum faction {
	GERMANIC,
	ROMAN,
	SOVIET,
	LAST_FACTION
};

const std::string FONT[] = {"res/Autobahn.ttf", "res/Roman SD.ttf", "res/Red October.ttf"},
	NAME[] = {"German Reich", "SPQR", "Soviet Socialist Union"};

const SDL_Color COLOR_TITLE[] = {{0x0, 0x0, 0x0}, {0xFF, 0xF1, 0x0}, {0xFF, 0xF1, 0x0}},
	COLOR_FG[] = {{0xDB, 0x01, 0x00}, {0xFF, 0xF1, 0x0}, {0xFB, 0xF1, 0xF0}},
	COLOR_BG[] = {{0xFD, 0xCD, 0x01}, {0xFD, 0x0B, 0x14}, {0xED, 0x1B, 0x24}};

const int SIZE_TITLE[] = {80, 40, 44},
	SIZE_FG[] = {50, 24, 24};

const Uint16 PORT_CLIENT = 52757,
	PORT_SERVER = 52657;

const unsigned int BUFFER_SIZE = 512;

const std::string VERSION = "c0.0";
