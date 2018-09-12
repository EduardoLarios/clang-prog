#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
    pid_t pid, pid1;
    /* fork a child process */
    pid = fork();
    printf("PID: %d\n", getpid());
    printf("PID: %d\n", pid1);

    if (pid < 0)
    { /* error ocurred */
        perror(argv[0]);
        return -1;
    }
    else if (pid == 0)
    { /* child process */
        pid1 = getpid();
        printf("A child - pid = %d \n ", pid);  /* A */
        printf("B child - pid = %d \n ", pid1); /* B */
        return 0;
    }
    else
    { /* parent process */
        pid1 = getpid();
        printf("C parent - pid = %d \n ", pid);  /* C */
        printf("D parent - pid = %d \n ", pid1); /* D */
        /* parent will wait for the child to complete */
        wait(NULL);
    }
    return 0;
}