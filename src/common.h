#pragma once

#include <arpa/inet.h>
#include <chrono>
#include <cstring>
#include <ctime>
#include <iostream>
#include <math.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

const unsigned int screen_width = 1366, screen_height = 768;

enum faction { GOTH, ROMAN, CELT, LAST_FACTION };

const std::string FONT[] = {"res/Autobahn.ttf", "res/Roman SD.ttf",
                            "res/Red October.ttf"},
                  NAME[] = {"Goths", "Roman Empire", "Celts"};

const SDL_Color
    COLOR_TITLE[] = {{0x0, 0x0, 0x0}, {0xFF, 0xF1, 0x0}, {0xFE, 0xF1, 0x0}},
    COLOR_FG[] = {{0xDB, 0x01, 0x00}, {0xFF, 0xF1, 0x0}, {0xFB, 0xF1, 0xF0}},
    COLOR_BG[] = {{0xFD, 0xCD, 0x01}, {0xFD, 0x0B, 0x14}, {0xED, 0x1B, 0x24}};

const int SIZE_TITLE[] = {80, 40, 44}, SIZE_FG[] = {50, 24, 24};

const Uint16 PORT_CLIENT = 52757, PORT_SERVER = 52657;

const unsigned int BUFFER_SIZE = 512;

const std::string VERSION = "c0.0";

const double PI = 3.14159265359;

/*packet rules*/
/* 0 X X X X - network info
   0 6 0 0 0 - ACK packet
   1 F X X X - client joining (F - theme/faction)
   2 x y z 0 - 32 bit - set camera (x - x coord, y - y coord, z - zoom * 32)
   3 i e x y a h s - 32 bit - repeating - visible entity (i - id, e - entity
   type, x - x coord, y - y coord, a - angle in radians * 1024, h - health, s -
   stamina)
   4 d 0 X X - tell client what map is being used (d - c string showing the
   location of the map image)
*/

const uint8_t ACK[] = {0, 6};
