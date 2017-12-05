/*
  For containing things relating to communication with the server
  including the receiver loop
*/

#include "common.h"
#include "entity.h"

int receiver_init (int& sockfd, sockaddr_in& sock_addr)
{
	int return_value = 0;

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {	//create socket
		std::cout << "ERROR: could not create socket\n";
		return_value = -1;
	} else {
		std::memset((char *) &sock_addr, 0, sizeof(sock_addr));	//zero addr_s
		sock_addr.sin_family = AF_INET;
		sock_addr.sin_port = htons(PORT_CLIENT);
		sock_addr.sin_addr.s_addr = INADDR_ANY;
		//bind socket
		if (bind(sockfd, (struct sockaddr *)&sock_addr, sizeof(sock_addr)) < 0) {
			std::cout << "ERROR: could not bind socket\n";
			return_value = -2;
		}
	} 

	return return_value;}

int sender_init (int& sockfd, sockaddr_in& sock_addr, char* server_ip)
{
	int return_value = 0;

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {	//create socket
		std::cout << "ERROR: could not create socket\n";
		return_value = -1;
	} else {
		std::memset((char *) &sock_addr, 0, sizeof(sock_addr));	//zero addr_s
		sock_addr.sin_family = AF_INET;
		sock_addr.sin_port = htons(PORT_SERVER);
		sock_addr.sin_addr.s_addr = inet_addr(server_ip);
	} 

	return return_value;
}

void receiver_loop (bool& quit, std::vector<visual_entity>& render_list)
{
	int sockfd;
	sockaddr_in sock_addr;

	receiver_init(sockfd, sock_addr);

	unsigned int socklen = sizeof(sock_addr);
	char buffer[BUFFER_SIZE];
	std::memset(buffer, 0, BUFFER_SIZE);
	while (!quit) {
		recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&sock_addr, &socklen);
	}
}

void simple_send (int sockfd, sockaddr_in sock_addr, char* data)
{
	unsigned int socklen;
	socklen = sizeof(sock_addr);
	char buffer[BUFFER_SIZE];
	std::memset(buffer, 0, BUFFER_SIZE);
	strcpy(buffer, data);
	sendto(sockfd, buffer, BUFFER_SIZE, 0, (sockaddr *)&sock_addr, socklen);
}
