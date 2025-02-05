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
    printf("TCP server closed, check for file\n");

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork failed");
        return 1;
    }
    else if (pid == 0)
    {
        execl("build/src/judge", "judge", (char *)NULL);
        perror("execl failed");
        return 1;
    }
    else
    {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status))
        {
            printf("judge process exited with status %d\n", WEXITSTATUS(status));
        }
        else
        {
            printf("judge process exited abnormally\n");
        }
    }
    printf("Judge finished, bye\n");
    return 0;
}
