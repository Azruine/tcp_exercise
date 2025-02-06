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

#define TEMP_OUTPUT "temp/temp_output"
#define IO_DIR "io"

/**
 * @brief Compile the submission.
 * @param source_path path to the source file.
 * @param executable_path path to the compiled executable.
 */
int compile_submission(const char *source_path, const char *executable_path)
{
    char command[512];
    snprintf(command, sizeof(command), "gcc %s -o %s", source_path, executable_path);
    printf("compile command: %s\n", command);
    int ret = system(command);
    return ret;
}

/**
 * @brief Run the compiled submission against a test case.
 *
 * @param in_path path to the input file.
 * @param expected_out path to the expected output file.
 * @param exec_time execution time in ms (output).
 * @param max_rss used memory (output).
 * @param executable_path path to the compiled executable.
 * @return 1 if test passed, 0 if failed.
 */
int run_test(const char *in_path, const char *expected_out, int *exec_time, long *max_rss, const char *executable_path)
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

        execl(executable_path, "solution", (char *)NULL);
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

        *max_rss = usage.ru_maxrss;

        int utime_ms = usage.ru_utime.tv_sec * 1000 + usage.ru_utime.tv_usec / 1000;
        int stime_ms = usage.ru_stime.tv_sec * 1000 + usage.ru_stime.tv_usec / 1000;
        *exec_time = utime_ms + stime_ms;

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

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <source_file_path>\n", argv[0]);
        return 1;
    }
    const char *source_path = argv[1];

    const char *base = strrchr(source_path, '/');
    if (base)
        base++;
    else
        base = source_path;

    char base_name[256];
    strncpy(base_name, base, sizeof(base_name));
    base_name[sizeof(base_name) - 1] = '\0';
    char *dot = strrchr(base_name, '.');
    if (dot)
    {
        *dot = '\0';
    }
    char executable_path[256];
    snprintf(executable_path, sizeof(executable_path), "temp/%s", base_name);

    if (compile_submission(source_path, executable_path) != 0)
    {
        printf("compile failed\n");
        return 1;
    }

    DIR *dir = opendir(IO_DIR);
    if (!dir)
    {
        perror("opendir failed");
        remove(executable_path);
        return 1;
    }

    struct dirent *entry;
    int max_total_time = 0;
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

                int exec_time = 0;
                long mem_usage = 0;
                int test_result = run_test(in_path, expected_path, &exec_time, &mem_usage, executable_path);
                if (!test_result)
                {
                    all_passed = 0;
                }
                else
                {
                    if (exec_time > max_total_time)
                        max_total_time = exec_time;
                    if (mem_usage > max_total_rss)
                        max_total_rss = mem_usage;
                }
            }
        }
    }
    closedir(dir);

    if (remove(executable_path) != 0)
    {
        perror("remove compiled executable failed");
    }

    if (all_passed)
    {
        printf("\nAccepted\n");
        printf("time: %d ms, memory: %ld KB\n", max_total_time, max_total_rss);
    }
    else
    {
        printf("\nWrong Answer\n");
    }

    return 0;
}
