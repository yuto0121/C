#include "logger.h"
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

static int pipe_fd[2];
static pid_t logger_pid = -1;

int create_log_process()
{
    if (pipe(pipe_fd) == -1)
    {
        perror("failure:pipe");
        return -1;
    }

    logger_pid = fork();

    if (logger_pid == -1)
    {
        perror("failure:fork");
        return -1;
    }

    if (logger_pid == 0)
    {
        close(pipe_fd[1]);
        FILE *pipe_read = fdopen(pipe_fd[0], "r");
        if (pipe_read == NULL)
        {
            perror("failure:fdopen");
            exit(1);
        }

        FILE *log_file = fopen("gateway.log", "a");
        if (log_file == NULL)
        {
            perror("failure:fopen");
            exit(1);
        }

        int seq_num = 0;
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), pipe_read) != NULL)
        {
            buffer[strcspn(buffer, "\n")] = '\0';

            if (strcmp(buffer, "END") == 0)
            {
                break;
            }

            time_t now = time(NULL);
            char *timestamp = ctime(&now);
            timestamp[strlen(timestamp) - 1] = '\0';

            fprintf(log_file, "%d - %s - %s\n", seq_num, timestamp, buffer);
            fflush(log_file);

            seq_num++;
        }

        fclose(log_file);
        fclose(pipe_read);
        exit(0);
    }
    else
    {
        close(pipe_fd[0]);
    }
    return 0;
}

int write_to_log_process(char *msg)
{
    if (pipe_fd[1] != -1)
    {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%s\n", msg);
        write(pipe_fd[1], buffer, strlen(buffer));
    }
    else
    {
        fprintf(stderr, "Logger's pipe is not initialized\n");
        return -1;
    }
    return 0;
}

int end_log_process()
{
    write_to_log_process("END");

    close(pipe_fd[1]);
    pipe_fd[1] = -1;

    int status;
    waitpid(logger_pid, &status, 0);

    return 0;
}
