#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "tcp/tcp_server.h"
#include "defineshit.h"

int main(void)
{
    printf("Start TCP server\n");
    start_tcp_server();
    printf("TCP server closed, bye\n");
    return 0;
}
