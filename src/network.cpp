/*
  For containing things relating to communication with the server
  including the receiver loop
*/

#include "common.h"

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
