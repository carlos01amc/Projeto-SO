#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define MAX 1024
#define SIZE 400

typedef struct config
{
    char *func;
    int max;
    int current;
} Config;

int push(char queue[SIZE][200], int *top, char data[SIZE])
{
    if (*top == SIZE - 1)
        return (-1);
    else
    {
        *top = *top + 1;
        strcpy(queue[*top], data);
        return (1);
    }
}

int pop(char queue[SIZE][200], int *top, char data[SIZE])
{
    if (*top == -1)
        return (-1);
    else
    {
        strcpy(data, queue[*top]);
        *top = *top - 1;
        return (1);
    }
}

int main(int argc, char const *argv[])
{
    while (1)
    {
        char *prog = malloc(sizeof(int) * 100);

        char *path_trans = malloc(25);
        char aux[50];
        strcpy(aux, argv[2]);
        path_trans = strcat(aux, "/");

        char *buffer = malloc(sizeof(int) * MAX);
        char queue[SIZE][200], data[SIZE];
        char *line;

        char exec[10];

        char *trans = malloc(sizeof(int) * MAX);

        int config_file = open(argv[1], O_RDONLY);

        Config config[7];

        read(config_file, trans, MAX);

        close(config_file);

        char *conf;
        conf = strtok(trans, " ");
        int x = 0;
        while (conf != NULL)
        {
            config[x].func = conf;
            conf = strtok(NULL, " \n");
            config[x].max = atoi(conf);
            conf = strtok(NULL, " \n");
            x += 1;
        }

        int fd_exec = open("pipe_exec", O_RDONLY);

        if (read(fd_exec, buffer, MAX) == -1)
        {
            perror("error reading pipe exec");
        }

        strcpy(exec, buffer);
        free(buffer);
        close(fd_exec);

        if (strcmp(exec, "proc-file") == 0)
        {

            int main_pipe = open("main_pipe", O_RDONLY);
            char *buf = malloc(sizeof(int) * MAX);

            int next_pos = 0;

            if (read(main_pipe, buf, MAX) == -1)
            {
                perror("Error reading main pipe");
                return 3;
            }

            close(main_pipe);

            char *transf;
            transf = strtok(buf, " ");
            int size = 0;
            int top = -1;
            while (transf != NULL)
            {

                push(queue, &top, transf);
                transf = strtok(NULL, " ");

                size += 1;
            }

            free(buf);

            int queue_size = size - 2;
            int pipe_status = open("pipe_status", O_WRONLY);

            if (queue_size == 1)
            {
                for (int j = 0; j < 7; j++)
                {
                    if (strcmp(queue[2], config[j].func) == 0)
                    {
                        config[j].current += 1;

                        if (config[j].current > config[j].max)
                        {
                        }
                        else
                        {
                        }
                    }
                }

                write(pipe_status, "pending\n", strlen("pending\n"));

                int path_file = open(queue[0], O_RDONLY);
                int save_file = open(queue[1], O_WRONLY | O_TRUNC, 0666);

                ssize_t o_stdin = dup(0);
                ssize_t o_stdout = dup(1);

                dup2(path_file, STDIN_FILENO);
                close(path_file);
                dup2(save_file, STDOUT_FILENO);
                close(save_file);

                char path_trans_aux[50];
                strcpy(path_trans_aux, path_trans);
                char *trans_name;
                trans_name = strcat(path_trans_aux, queue[2]);

                int pid = fork();

                if (pid == 0)
                {
                    write(pipe_status, "processing\n", strlen("processing\n"));
                    execl(trans_name, queue[2], NULL);
                    perror("error in one ocasion");
                    exit(0);
                }
                int status;
                wait(&status);
                dup2(o_stdin, 0);
                dup2(o_stdout, 1);
            }

            else if (queue_size > 1)
            {
                write(pipe_status, "pending\n", strlen("pending\n"));

                int path_file_o = open(queue[0], O_RDONLY);
                int save_file_o = open(queue[1], O_WRONLY | O_TRUNC, 0666);

                int fd[queue_size - 1][2];

                for (int i = 0; i < queue_size - 1; i++)
                {
                    if (pipe(fd[i]) == -1)
                    {
                        perror("Error creating pipe");
                        return 1;
                    }
                }

                if (queue[2])
                {
                    for (int j = 0; j < 7; j++)
                    {
                        if (strcmp(queue[2], config[j].func) == 0)
                        {
                            config[j].current += 1;

                            if (config[j].current > config[j].max)
                            {
                            }
                            else
                            {
                            }
                        }
                    }
                    write(pipe_status, "processing\n", strlen("processing\n"));
                    char path_trans_aux[50];
                    strcpy(path_trans_aux, path_trans);
                    char *trans_name;
                    trans_name = strcat(path_trans_aux, queue[2]);

                    int path_file = dup2(path_file_o, STDIN_FILENO);
                    close(path_file_o);

                    int pid = fork();

                    if (pid == 0)
                    {
                        close(fd[0][0]);
                        dup2(fd[0][1], STDOUT_FILENO);
                        close(fd[0][1]);
                        execl(trans_name, queue[2], NULL);
                        close(path_file);
                        perror("error in first ocasion");
                        exit(0);
                    }
                    int status;
                    wait(&status);
                    close(fd[0][1]);
                }

                for (int i = 3, j = 1; i < size - 1; i++, j++)
                {

                    if (queue[i])
                    {
                        char path_trans_aux[50];
                        strcpy(path_trans_aux, path_trans);
                        char *trans_name;
                        trans_name = strcat(path_trans_aux, queue[i]);

                        for (int j = 0; j < 7; j++)
                        {
                            if (strcmp(queue[i], config[j].func) == 0)
                            {
                                config[j].current += 1;

                                if (config[j].current > config[j].max)
                                {
                                }
                                else
                                {
                                }
                            }
                        }

                        int pid = fork();

                        if (pid == 0)
                        {
                            close(fd[j][0]);
                            dup2(fd[j - 1][0], STDIN_FILENO);
                            close(fd[j - 1][0]);
                            dup2(fd[j][1], STDOUT_FILENO);
                            close(fd[j][1]);
                            execl(trans_name, queue[i], NULL);
                            perror("error in the middle of the transformation");
                            exit(0);
                        }
                        int status;
                        wait(&status);
                        close(fd[j - 1][0]);
                        close(fd[j][1]);
                    }
                }

                if (queue[size - 1])
                {

                    char path_trans_aux[50];
                    strcpy(path_trans_aux, path_trans);
                    char *trans_name;
                    trans_name = strcat(path_trans_aux, queue[size - 1]);

                    for (int j = 0; j < 7; j++)
                    {
                        if (strcmp(queue[size - 1], config[j].func) == 0)
                        {
                            config[j].current += 1;

                            if (config[j].current > config[j].max)
                            {
                            }
                            else
                            {
                            }
                        }
                    }

                    int pid = fork();

                    if (pid == 0)
                    {
                        dup2(fd[queue_size - 2][0], STDIN_FILENO);
                        close(fd[queue_size - 2][0]);

                        int save_file = dup2(save_file_o, STDOUT_FILENO);
                        close(save_file_o);
                        execl(trans_name, queue[size - 1], NULL);
                        close(save_file);
                        perror("error in last ocasion");
                        exit(0);
                    }
                    int status;
                    wait(&status);
                }

                close(fd[queue_size - 1][0]);
            }
            int file_bytes = 0;
            int save_bytes = 0;
            int file = open(queue[0], O_RDONLY);
            char c;
            while (read(file, &c, 1) == 1)
            {
                file_bytes++;
            }

            close(file);
            int save = open(queue[1], O_RDONLY);
            char s;
            int r_bytes = 0;
            while (read(save, &s, 1) == 1)
            {
                save_bytes++;
            }
            close(save);

            sprintf(prog, "concluded (bytes-input: %d, bytes-output: %d)\n", file_bytes, save_bytes);
            write(pipe_status, prog, strlen(prog));
            close(pipe_status);
        }

        if (strcmp(exec, "status") == 0)
        {
            int pipe_status = open("pipe_status", O_WRONLY);

            for (int i = 0; i < 7; i++)
            {
                write(pipe_status, "transf ", strlen("transf "));
                write(pipe_status, config[i].func, strlen(config[i].func));
                char str[10];
                sprintf(str, " %d/%d", config[i].current, config[i].max);
                write(pipe_status, str, sizeof(config[i].max));
                write(pipe_status, " (running/max)", strlen(" (running/max)"));
                write(pipe_status, "\n", 3);
            }

            close(pipe_status);
        }
    }

    return 0;
}