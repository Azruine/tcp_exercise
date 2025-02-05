#ifndef TCP_SERVER_H
#define TCP_SERVER_H
#ifndef SO_REUSEPORT
#define SO_REUSEPORT 15
#endif

/**
 * @brief start tcp server
 * @return int 0 on success, -1 on failure
 */
int start_tcp_server(void);

#endif // TCP_SERVER_H
