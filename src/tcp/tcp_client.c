#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <endian.h> // htobe64, be64toh (Linux)
#include "tcp_client.h"
#include "../defineshit.h"

#define HEADER_SIZE 16
#define TEXTFILE "TEXTFILE"

ssize_t send_all(int sockfd, const void *buffer, size_t length)
{
    size_t total_sent = 0;
    const char *buf = (const char *)buffer;
    while (total_sent < length)
    {
        ssize_t sent = send(sockfd, buf + total_sent, length - total_sent, 0);
        // Should not reach here...
        if (sent <= 0)
        {
            printf("send() failed\n");
            return -1;
        }
        total_sent += sent;
    }
    return total_sent;
}

int send_file(const char *server_ip, int port, const char *filename)
{
    int sockfd;
    struct sockaddr_in server_addr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket creation failed");
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0)
    {
        perror("inet_pton failed");
        close(sockfd);
        return -1;
    }

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect failed");
        close(sockfd);
        return -1;
    }

    FILE *fp = fopen(filename, "rb");
    if (!fp)
    {
        perror("fopen failed");
        close(sockfd);
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    uint64_t file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char header[HEADER_SIZE];
    memset(header, 0, HEADER_SIZE);
    memcpy(header, TEXTFILE, 8);
    uint64_t net_file_size = htobe64(file_size);
    memcpy(header + 8, &net_file_size, 8);

    if (send_all(sockfd, header, HEADER_SIZE) != HEADER_SIZE)
    {
        perror("failed to send header");
        fclose(fp);
        close(sockfd);
        return -1;
    }

    char buffer[1024];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0)
    {
        if (send_all(sockfd, buffer, bytes_read) != bytes_read)
        {
            perror("failed to send file");
            fclose(fp);
            close(sockfd);
            return -1;
        }
    }

    printf("file '%s' sent (size: %lu bytes)\n", filename, file_size);
    fclose(fp);

    char result_buf[4096];
    size_t total_received = 0;
    ssize_t r;

    while ((r = recv(sockfd, result_buf + total_received, sizeof(result_buf) - total_received - 1, 0)) > 0)
    {
        total_received += r;
        if (total_received >= sizeof(result_buf) - 1)
            break;
    }
    if (r < 0)
    {
        perror("recv failed");
        close(sockfd);
        return -1;
    }
    result_buf[total_received] = '\0';

    printf("Judge result received:\n%s\n", result_buf);

    close(sockfd);
    return 0;
}

#ifdef TEST_TCP_CLIENT
int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Usage: %s <server_ip> <port> <filename>\n", argv[0]);
        return 1;
    }
    const char *server_ip = argv[1];
    int port = atoi(argv[2]);
    const char *filename = argv[3];

    return send_file(server_ip, port, filename);
}
#endif
