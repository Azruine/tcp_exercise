#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "tcp_server.h"
#include "../defineshit.h"

#define PORT 49999
#define BACKLOG 5
/**
 * Header format:
 * 8 bytes: message type
 * 8 bytes: file size
 */
#define HEADER_SIZE 16

ssize_t read_multiple_bytes(int sockfd, void *buffer, size_t count)
{
    ssize_t nread = 0;
    ssize_t n;
    char *buf = (char *)buffer;

    while (nread < count)
    {
        n = read(sockfd, buf + nread, count - nread);

        if (n < 0)
            return -1;
        else if (n == 0)
            break;

        nread += n;
    }

    return nread;
}

int start_tcp_server(void)
{
    int server_fd, client_fd;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) == -1)
    {
        perror("setsockopt");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1)
    {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, BACKLOG) == -1)
    {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("TCP server waiting for client on port %d\n", PORT);

    if ((client_fd = accept(server_fd, (struct sockaddr *)&address, &addrlen)) == -1)
    {
        perror("accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Client connected: %s:%d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

    printf("Reading header\n");
    char header[HEADER_SIZE];
    if (read_multiple_bytes(client_fd, header, HEADER_SIZE) != HEADER_SIZE)
    {
        perror("read header failed");
        close(client_fd);
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    char msg_type[9];
    memcpy(msg_type, header, 8);
    msg_type[8] = '\0';

    uint64_t net_file_size;
    memcpy(&net_file_size, header + 8, 8);
    uint64_t file_size = be64toh(net_file_size);

    printf("Received header: %s, file size: %lu\n", msg_type, file_size);

    FILE *file = fopen("files/receive/received_file.c", "wb");
    if (file == NULL)
    {
        perror("fopen failed");
        close(client_fd);
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    uint64_t remaining_size = file_size;
    char buffer[1024];
    ssize_t nread;

    while (remaining_size > 0)
    {
        size_t read_size = remaining_size < sizeof(buffer) ? remaining_size : sizeof(buffer);
        nread = read_multiple_bytes(client_fd, buffer, read_size);

        if (nread < 0)
        {
            perror("read file failed");
            fclose(file);
            close(client_fd);
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        fwrite(buffer, 1, nread, file);
        remaining_size -= nread;
    }

    if (remaining_size != 0)
    {
        perror("file size mismatch");
        fclose(file);
        close(client_fd);
        close(server_fd);
        exit(EXIT_FAILURE);
    }   
    else 
    {
        printf("File received successfully\n");
    }

    fclose(file);

    close(client_fd);
    close(server_fd);

    return 0;
}
