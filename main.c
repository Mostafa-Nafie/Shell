#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>


#define MAX_LENGTH 100
#define MAX_WORDS 10

void take_user_input(char* line)
{
    fgets(line, MAX_LENGTH, stdin);
}

void parse_input(char* line, char* arr[])
{
    int j = 0;
    arr[j] = strtok(line , " \n");
        while(arr[j]!= NULL)
        {
            j++;
            arr[j] = strtok(NULL, " \n");
        }
}

int pipe_search(char* tokens[])
{
    for(int i = 1; i < MAX_WORDS && tokens[i] != '\0'; i++)
    {
        if(strcmp(tokens[i], "|") == 0)
            return i;
    }
    return 0;
}

void piping(char* tokens[], int pipe_loc)
{
    int fd[2];
    pipe(fd);
    char* pipe1[MAX_WORDS];
    char* pipe2[MAX_WORDS];

    for(int i = 0; i < pipe_loc; i++)
        pipe1[i] = tokens[i];
    pipe1[pipe_loc] = NULL;

    int j = 0;
    for(int i = pipe_loc + 1; j < MAX_WORDS && tokens[i] != '\0'; i++, j++)
        pipe2[j] = tokens[i];
    pipe2[j] = NULL;

    pid_t pid = fork();
    if(pid == 0)
    {
        dup2(fd[1], 1);
        close(fd[0]);
        execvp(pipe1[0], pipe1);
    }
    else
    {
        dup2(fd[0], 0);
        close(fd[1]);
        execvp(pipe2[0], pipe2);
        wait(NULL);
    }
}

int main()
{
    char input[MAX_LENGTH];
    char* tokens[MAX_WORDS];

    while(1)
    {
        take_user_input(input);

        if(strcmp(input, "exit\n") == 0)
            break;

        parse_input(input, tokens);
        int pipe_loc = pipe_search(tokens);

        if(strcmp(tokens[0], "cd") == 0)
        {
            if(tokens[1] == NULL)
                printf("%s", "Expeceted an argument to cd\n");
            else
                chdir(tokens[1]);
        }
        else
        {
            pid_t pid = fork();
            if(pid == 0)
            {
                if(pipe_loc != 0)
                    piping(tokens, pipe_loc);
                else
                    execvp(tokens[0], tokens);
            }
            else
            {
                wait(NULL);
            }
        }
    }
}
