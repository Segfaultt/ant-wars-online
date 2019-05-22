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

const std::string FONT = "res/Roman SD.ttf";

const Uint16 PORT_CLIENT = 52390, PORT_SERVER = 52053;

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
   4 d 0 X X - tell client what map is being used (d - cstring showing the
   location of the map image)
*/

const uint8_t ACK[] = {0, 6};
