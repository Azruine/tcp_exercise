#ifndef TCP_SERVER_H
#define TCP_SERVER_H

/**
 * Send text to the client
 * @param client_fd: client file descriptor
 * @param text: text to send
 * @return 0 on success, -1 on failure
 */
int send_text_result(int client_fd, const char *text);

/**
 * Read multiple bytes from the socket
 * @param sockfd: socket file descriptor
 * @param buffer: buffer to store the data
 * @param count: number of bytes to read
 * @return number of bytes read on success, -1 on failure
 */
ssize_t read_multiple_bytes(int sockfd, void *buffer, size_t count);

/**
 * @brief start tcp server
 * @return int 0 on success, -1 on failure
 */
int start_tcp_server(void);

#endif // TCP_SERVER_H
