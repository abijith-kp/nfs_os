
void get_server(struct sockaddr_in *server, char *addr, int port, int *sock);
int send_msg(int sock, char *msg, int len);
int read_msg(int sock, char *msg, int len);
