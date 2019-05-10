int receiver_init(int &sockfd, sockaddr_in &sock_addr);

// void receiver_loop (bool &quit, std::vector<visual_entity> &render_list);

int sender_init(int &sockfd, sockaddr_in &sock_addr, char *server_ip);

void simple_send(int sockfd, sockaddr_in sock_addr, uint8_t *data,
                 int size = BUFFER_SIZE);

void uint32_to_uint8_t(uint32_t n, uint8_t *bytes, int &a);

uint32_t uint8_t_to_uint32(uint8_t *buffer, int &a);

void simple_receive(int sockfd, sockaddr_in &sock_addr, uint8_t *buffer);

bool wait_for_ack(int sockfd, sockaddr_in &sock_addr);

bool ack(int sockfd, sockaddr_in &sock_addr);
