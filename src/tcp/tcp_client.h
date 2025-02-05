#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

/**
 * @brief send file to server
 * @param server_ip server ip address in string format. i.e. "127.0.0.1"
 * @param port server port number
 * @param filename file name to send
 */
int send_file(const char *server_ip, int port, const char *filename);

#endif // TCP_CLIENT_H