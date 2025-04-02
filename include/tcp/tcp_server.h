#pragma once
#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <arpa/inet.h>
#include <endian.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "../defineshit.h"

#define PORT 49999
#define BACKLOG 5
#define HEADER_SIZE 16
#define BUFFER_SIZE 1024
#define JUDGE_RESULT_SIZE 1024

// client connection state
typedef enum {
    STATE_READING_HEADER,
    STATE_READING_FILE,
    STATE_WAIT_JUDGE,
    STATE_SENDING_RESULT,
    STATE_DONE
} conn_state;

/**
 * @brief client connection structure
 */
typedef struct client_conn {
    int fd;                    // client socket file descriptor
    struct sockaddr_in addr;   // client address
    conn_state state;          // client connection state
    char header[HEADER_SIZE];  // header buffer
    size_t header_bytes;       // byte size of the header received
    uint64_t file_size;        // byte size of the file to send
    uint64_t file_received;    // byte size of the file received
    FILE *fp;                  // file pointer for the received file
    int judge_pipe_fd;         // pipe file descriptor for the judge process /
                               // non-blocking
    char judge_result[JUDGE_RESULT_SIZE];  // judge result buffer
    size_t judge_result_len;               // judge result byte size
    size_t judge_sent;                     // byte size of the judge result sent
    char source_filename[256];             // source file name
    struct client_conn *next;              // next client connection
} client_conn;

/**
 * @brief signal handler for SIGINT
 * @param signum signal number
 */
void sigint_handler(int signum);

/**
 * @brief signal handler for SIGCHLD, prevent zombie process
 * @param signo signal number
 */
void sigchld_handler(int signo);

/**
 * @brief Start the TCP server
 * @return 0 on success, exit() on fatal error.
 */
int start_tcp_server(int port);

#endif  // TCP_SERVER_H
