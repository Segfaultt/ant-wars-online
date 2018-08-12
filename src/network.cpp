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
		//bind socket
		if (bind(sockfd, (struct sockaddr *)&sock_addr, sizeof(sock_addr)) < 0) {
			std::cout << "ERROR: could not bind socket\n";
			return_value = -2;
		}
	} 

	return return_value;
}

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

void simple_receive (int sockfd, sockaddr_in& sock_addr, char* buffer)
{
	unsigned int socklen;
	socklen = sizeof(sock_addr);
	std::memset(buffer, 0, BUFFER_SIZE);
	if (recvfrom(sockfd, buffer, 512, 0, (struct sockaddr *)&sock_addr, &socklen) < 0) //receive packet
		std::cout << "recvfrom failed" << std::endl;
}


bool wait_for_ack(int sockfd, sockaddr_in& sock_addr)
{
	char buffer[BUFFER_SIZE];
	std::memset(buffer, 0, BUFFER_SIZE);
	simple_receive(sockfd, sock_addr, buffer);

	return 0;
}

void ack(int sockfd, sockaddr_in& sock_addr)
{
	char buffer[2];
	strcpy(buffer, ACK);
	simple_send(sockfd, sock_addr, buffer);
}

void uint32_to_char(uint32_t n, uint8_t *buffer, int &a)
{
	buffer[a  ] = (n>>24) & 0xFF;
	buffer[a+1] = (n>>16) & 0xFF;
	buffer[a+2] = (n>>8)  & 0xFF;
	buffer[a+3] = n       & 0xFF;
	a += 4;
}

uint32_t char_to_uint32(uint8_t *buffer, int &a)
{
	uint32_t r = 0;
	r += buffer[a  ]<<24;
	r += buffer[a+1]<<16;
	r += buffer[a+2]<<8;
	r += buffer[a+3];
	a += 4;
	
	return r;
}
