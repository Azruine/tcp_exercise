#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <unistd.h>
#include <errno.h>
#include "defineshit.h"

#define SOURCE_PATH "files/received/received_file.c"
#define EXECUTABLE_PATH "temp/solution"
#define TEMP_OUTPUT "temp/temp_output"
#define IO_DIR "io"

/**
 * @brief Compile the submission
 * @return result of the compilation
 */
int compile_submission()
{
    char command[512];
    snprintf(command, sizeof(command), "gcc %s -o %s", SOURCE_PATH, EXECUTABLE_PATH);
    printf("compile command: %s\n", command);
    int ret = system(command);
    return ret;
}

/**
 * @brief Run the compiled submission
 * @param in_path path to the input file
 * @param expected_out path to the expected output file
 * @param exec_time execution time
 * @param max_rss used memory
 * @return result of the execution, 1 for true, 0 for false
 */
int run_test(const char *in_path, const char *expected_out, double *exec_time, long *max_rss)
{
    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork failed");
        return 0;
    }
    else if (pid == 0)
    {
        FILE *fin = fopen(in_path, "r");
        if (!fin)
        {
            perror("fopen failed");
            exit(1);
        }

        int fd_in = fileno(fin);

        if (dup2(fd_in, STDIN_FILENO) == -1)
        {
            perror("dup2(stdin) failed");
            exit(1);
        }
        fclose(fin);

        FILE *fout = fopen(TEMP_OUTPUT, "w");
        if (!fout)
        {
            perror("fopen failed");
            exit(1);
        }
        int fd_out = fileno(fout);
        if (dup2(fd_out, STDOUT_FILENO) == -1)
        {
            perror("dup2(stdout) failed");
            exit(1);
        }
        fclose(fout);

        execl(EXECUTABLE_PATH, "solution", (char *)NULL);
        perror("execl failed");
        exit(1);
    }
    else
    {
        struct rusage usage;
        int status;
        if (wait4(pid, &status, 0, &usage) == -1)
        {
            perror("wait4 failed");
            return 0;
        }
        FILE *f1 = fopen(expected_out, "r");
        FILE *f2 = fopen(TEMP_OUTPUT, "r");
        if (!f1 || !f2)
        {
            perror("fopen failed");
            return 0;
        }
        int result = 1;
        char buf1[1024], buf2[1024];
        while (fgets(buf1, sizeof(buf1), f1) && fgets(buf2, sizeof(buf2), f2))
        {
            if (strcmp(buf1, buf2) != 0)
            {
                result = 0;
                break;
            }
        }
        if (fgets(buf1, sizeof(buf1), f1) || fgets(buf2, sizeof(buf2), f2))
        {
            result = 0;
        }

        fclose(f1);
        fclose(f2);
        return 0;
    }
}

int main(void)
{
    printf("judge started\n");
    printf("compile submission\n");
    int ret = compile_submission();
    if (ret != 0)
    {
        printf("compile error\n");
        return 1;
    }
    printf("compile success\n");

    DIR *dir = opendir(IO_DIR);

    if (!dir)
    {
        perror("opendir failed");
        return 1;
    }

    struct dirent *entry;
    double exec_time;
    long max_rss;
    int passed = 1;



}