// Eduardo Larios Fernández
// A00569364 - ISC
// Programación Avanzada
// 02/Mar/2018

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <wait.h>

char *com_0;
char *com_1;
char *com_2;
char *com_3;
int arguments;

void caller(int sign)
{

    int pid;
    // execlp("ls","ls", "-l" ..., (char *) 0)
    // execvp(argv[1], &argv[1])

    if ((pid = fork()) < 0)
    {
        perror(com_0);
        exit(-1);
    }
    else if (pid == 0)
    {

        switch (sign)
        {
        // call with kill -s 1 pid 
        case 1:
            if (arguments == 4)
            {
                execlp(com_1, com_1, (char *)0);
            }
            else
            {
                execlp("ls", "ls", (char *)0);
            }
            break;
        // call with kill -s 2 pid
        case 2:
            if (arguments == 4)
            {
                execlp(com_2, com_2, (char *)0);
            }
            else
            {
                execlp("ps", "ps", (char *)0);
            }
            break;
        // call with kill -s 3 pid
        case 3:
            if (arguments == 4)
            {
                execlp(com_3, com_3, (char *)0);
            }
            else
            {
                execlp("pwd", "pwd", (char *)0);
            }
            break;
        }
    }
    else
    {
        wait(NULL);
    }
}

int main(int argc, char *argv[])
{

    if (argc != 1 && argc != 4)
    {
        fprintf(stderr, "Usage: %s command1 command2 command3\n", argv[0]);
        fprintf(stderr, "Usage: %s\n", argv[0]);
        return -1;
    }

    com_0 = argv[0];
    com_1 = argv[1];
    com_2 = argv[2];
    com_3 = argv[3];
    arguments = argc;

    // kill -s SIGKILL pid - Kill the process
    signal(1, caller);
    signal(2, caller);
    signal(3, caller);

    while (1)
    {
        printf("PID %i - Waiting for a signal\n", getpid());
        pause();
    }

    return 0;
}
