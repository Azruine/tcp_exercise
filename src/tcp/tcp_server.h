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
 * @brief start tcp server
 * @return int 0 on success, -1 on failure
 */
int start_tcp_server(void);

#endif // TCP_SERVER_H
