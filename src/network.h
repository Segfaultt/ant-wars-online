#include "entity.h"

int receiver_init (int& sockfd, sockaddr_in& sock_addr);

void receiver_loop (bool& quit, std::vector<visual_entity>& render_list);

int sender_init (int& sockfd, sockaddr_in& sock_addr, char* server_ip);

void simple_send (int sockfd, sockaddr_in sock_addr, char* data);

void simple_receive (int sockfd, sockaddr_in& sock_addr, char* buffer);
