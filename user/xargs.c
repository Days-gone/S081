#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

#define BUFSIZE 512
#define ARGLENGTH 100

char buf[BUFSIZE];
char *sub_args[MAXARG];

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        printf("should have a command\n");
        exit(0);
    }

    int new_arg_idx = 0;
    for (new_arg_idx = 0; new_arg_idx < argc - 1; new_arg_idx++)
    {
        char *cmdtemp = malloc(ARGLENGTH * sizeof(char));
        strcpy(cmdtemp, argv[new_arg_idx + 1]);
        sub_args[new_arg_idx] = cmdtemp;
    }

    while (gets(buf, BUFSIZE)[0] != 0)
    {
        char *temp = malloc(ARGLENGTH * sizeof(char));
        strcpy(temp, buf);
        temp[strlen(temp) - 1] = 0;
        sub_args[new_arg_idx] = temp;

        int pid = fork();
        if (pid < 0)
        {
            printf("[ERROR]:xargs cannot fork\n");
            exit(0);
        }
        if (pid == 0)
        {
            exec(sub_args[0], sub_args);
            exit(0);
        }
        else
        {
            wait((int *)0);
        }
        free(sub_args[new_arg_idx]);
    }
    exit(0);
}

/*
 args from the commandline is without \n as the end of a string
 args from the std-input is with \n as the end of a string
*/