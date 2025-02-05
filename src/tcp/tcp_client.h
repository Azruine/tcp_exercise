#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

/**
 * @brief call send() in a loop until all data is sent
 * @param sockfd socket file descriptor
 * @param buffer data to send
 * @param length data length
 * @return total bytes sent, or -1 on error
 */
ssize_t send_all(int sockfd, const void *buffer, size_t length);

/**
 * @brief send a file to the server
 * @param server_ip server IP address
 * @param port server port number
 * @param filename file to send
 * @return 0 on success, -1 on error
 */
int send_file(const char *server_ip, int port, const char *filename);

#endif // TCP_CLIENT_H