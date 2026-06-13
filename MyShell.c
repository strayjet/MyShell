#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

#define MAX_CMDS 10
#define MAX_ARGS 100
#define MAX_LEN 1024

int main()
{
    signal(SIGINT, SIG_IGN);

    char history[100][MAX_LEN];
    int history_count = 0;

    while (1)
    {
        // ---------------- INPUT ----------------
        char command[MAX_LEN];

        printf("myshell> ");

        if (fgets(command, sizeof(command), stdin) == NULL)
            break;

        command[strcspn(command, "\n")] = '\0';

        if (command[0] == '\0')
            continue;

        // ---------------- HISTORY STORE ----------------
        if (strcmp(command, "history") != 0)
        {
            if (history_count < 100)
            {
                strcpy(history[history_count], command);
                history_count++;
            }
        }

        // ---------------- BUILT-IN: EXIT ----------------
        if (strcmp(command, "exit") == 0)
            break;

        // ---------------- BUILT-IN: CD ----------------
        if (strncmp(command, "cd", 2) == 0)
        {
            char *path = command + 2;

            while (*path == ' ')
                path++;

            if (*path == '\0')
            {
                fprintf(stderr, "cd : missing arguments\n");
            }
            else
            {
                if (chdir(path) != 0)
                    perror("cd");
            }
            continue;
        }

        // ---------------- BUILT-IN: PWD ----------------
        if (strcmp(command, "pwd") == 0)
        {
            char cwd[1024];

            if (getcwd(cwd, sizeof(cwd)) != NULL)
                printf("%s\n", cwd);
            else
                perror("pwd");

            continue;
        }

        // ---------------- BUILT-IN: HISTORY ----------------
        if (strcmp(command, "history") == 0)
        {
            for (int i = 0; i < history_count; i++)
                printf("%d %s\n", i + 1, history[i]);

            continue;
        }

        // ---------------- BUILT-IN: !! ----------------
        if (strcmp(command, "!!") == 0)
        {
            if (history_count == 0)
            {
                printf("No commands in history\n");
                continue;
            }

            strcpy(command, history[history_count - 1]);
            printf("%s\n", command);
        }

        // ---------------- PARSING ----------------
        char *cmds[MAX_CMDS][MAX_ARGS];
        int argc[MAX_CMDS];

        for (int i = 0; i < MAX_CMDS; i++)
            argc[i] = 0;

        int cmd_index = 0;

        char *token = strtok(command, " ");

        char *input_file = NULL;
        char *output_file = NULL;
        int append_mode = 0;
        int background = 0;

        while (token != NULL)
        {
            if (strcmp(token, "|") == 0)
            {
                cmds[cmd_index][argc[cmd_index]] = NULL;
                cmd_index++;

                if (cmd_index >= MAX_CMDS)
                {
                    printf("Too many commands\n");
                    break;
                }
            }
            else if (strcmp(token, "<") == 0)
            {
                input_file = strtok(NULL, " ");

                if (input_file == NULL)
                {
                    printf("Missing input file\n");
                    break;
                }
            }
            else if (strcmp(token, ">") == 0)
            {
                append_mode = 0;

                output_file = strtok(NULL, " ");

                if (output_file == NULL)
                {
                    printf("Missing output file\n");
                    break;
                }
            }
            else if (strcmp(token, ">>") == 0)
            {
                append_mode = 1;

                output_file = strtok(NULL, " ");

                if (output_file == NULL)
                {
                    printf("Missing output file\n");
                    break;
                }
            }
            else if (strcmp(token, "&") == 0)
            {
                background = 1;
            }
            else
            {
                cmds[cmd_index][argc[cmd_index]++] = token;
            }

            token = strtok(NULL, " ");
        }

        cmds[cmd_index][argc[cmd_index]] = NULL;

        int num_cmds = cmd_index + 1;
        int num_pipes = num_cmds - 1;

        if (cmds[0][0] == NULL)
            continue;

        // ---------------- PIPE CREATION ----------------
        int pipes[num_pipes][2];

        for (int i = 0; i < num_pipes; i++)
        {
            if (pipe(pipes[i]) < 0)
            {
                perror("pipe failed");
                exit(1);
            }
        }

        // ---------------- PROCESS CREATION ----------------
        pid_t pid[num_cmds];

        for (int i = 0; i < num_cmds; i++)
        {
            pid[i] = fork();

            if (pid[i] < 0)
            {
                perror("fork failed");
                exit(1);
            }

            if (pid[i] == 0)
            {
                signal(SIGINT, SIG_DFL);

                // input pipe
                if (i > 0)
                    dup2(pipes[i - 1][0], STDIN_FILENO);

                // output pipe
                if (i < num_cmds - 1)
                    dup2(pipes[i][1], STDOUT_FILENO);

                // close pipes
                for (int j = 0; j < num_pipes; j++)
                {
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }

                // input redirection
                if (input_file && i == 0)
                {
                    int fd = open(input_file, O_RDONLY);

                    if (fd < 0)
                    {
                        perror("open");
                        exit(1);
                    }

                    dup2(fd, STDIN_FILENO);
                    close(fd);
                }

                // output redirection
                if (output_file && i == num_cmds - 1)
                {
                    int fd;

                    if (append_mode)
                    {
                        fd = open(output_file,
                                  O_WRONLY | O_CREAT | O_APPEND,
                                  0644);
                    }
                    else
                    {
                        fd = open(output_file,
                                  O_WRONLY | O_CREAT | O_TRUNC,
                                  0644);
                    }

                    if (fd < 0)
                    {
                        perror("open");
                        exit(1);
                    }

                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                }

                execvp(cmds[i][0], cmds[i]);

                perror("execvp failed");
                exit(1);
            }
        }

        // ---------------- CLEANUP ----------------
        for (int i = 0; i < num_pipes; i++)
        {
            close(pipes[i][0]);
            close(pipes[i][1]);
        }

        if (!background)
        {
            for (int i = 0; i < num_cmds; i++)
                waitpid(pid[i], NULL, 0);
        }
    }

    return 0;
}
