#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <unistd.h>
#include <errno.h>
#include "../defineshit.h"

#define SOURCE_PATH "files/receive/received_file.c"
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
        return result;
    }
}

int main(void)
{
    // printf("compile submission\n");
    if (compile_submission() != 0)
    {
        printf("compile failed\n");
        return 1;
    }
    // printf("compile success: executable file created -> %s\n", EXECUTABLE_PATH);
    DIR *dir = opendir(IO_DIR);
    if (!dir)
    {
        perror("opendir failed");
        return 1;
    }

    struct dirent *entry;
    double max_total_time = 0;
    long max_total_rss = 0;
    int all_passed = 1;

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_REG)
        {
            char *ext = strrchr(entry->d_name, '.');
            if (ext && strcmp(ext, ".in") == 0)
            {
                char in_path[256];
                snprintf(in_path, sizeof(in_path), "%s/%s", IO_DIR, entry->d_name);

                char expected_output[256];
                strncpy(expected_output, entry->d_name, sizeof(expected_output));
                expected_output[sizeof(expected_output) - 1] = '\0';
                char *dot = strrchr(expected_output, '.');
                if (dot)
                {
                    strcpy(dot, ".out");
                }
                char expected_path[256];
                snprintf(expected_path, sizeof(expected_path), "%s/%s", IO_DIR, expected_output);

                // printf("test case: input = %s, expected output = %s\n", in_path, expected_path);
                double exec_time = 0;
                long mem_usage = 0;
                int test_result = run_test(in_path, expected_path, &exec_time, &mem_usage);
                if (!test_result)
                {
                    // printf("test case '%s' failed\n", entry->d_name);
                    all_passed = 0;
                }
                else
                {
                    // printf("test case '%s' passed, execution time: %.3f sec, memory usage: %ld KB\n", entry->d_name, exec_time, mem_usage);
                    if (exec_time > max_total_time)
                        max_total_time = exec_time;
                    if (mem_usage > max_total_rss)
                        max_total_rss = mem_usage;
                }
            }
        }
    }
    closedir(dir);

    if (all_passed)
    {
        printf("\nAccepted\n");
        printf("time: %.3f sec, memory: %ld KB\n", max_total_time, max_total_rss);
    }
    else
    {
        printf("\nWrong Answer\n");
    }

    return 0;
}