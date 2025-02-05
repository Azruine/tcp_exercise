#include <stdio.h>
#include "tcp/tcp_server.h"

int main(void)
{
    printf("Start TCP server\n");
    start_tcp_server();
    printf("End TCP server, bye\n");
    return 0;
}
