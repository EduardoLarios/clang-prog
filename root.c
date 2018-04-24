// Eduardo Larios Fernández
// A00569364 - ISC
// 09/Abril/2018
// Compile with: gcc root.c -o root -lpthread -lm

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>

#define SIZE 100

double array[SIZE];

void root()
{
    double i;

    for (i = 0.0; i < 100.0; i++)
    {
        array[(int)i] = sqrt(i);
    }
};

void *workload(void *param)
{
    int thread = *((int *)param);
    int i;
    if (thread)
    {
        root();
    }
    else
    {
        printf("Waiting for the result...\n");
        while (1)
        {
            if (array[SIZE - 1])
            {
                for (i = 0; i < 100; i++)
                {
                    printf("[ %lf ]\n", array[i]);
                }
                break;
            }
        }
    }

    return ((void *)0);
}

int main(int arg, char *argv[])
{
    pthread_t thread_id[2];
    void *val;
    int t = 1, t2 = 0;
    char message[] = "Please wait\n";

    printf("PID = %i - Threads are being created\n", getpid());
    pthread_create(&thread_id[0], NULL, workload, (void *)&t);
    pthread_create(&thread_id[1], NULL, workload, (void *)&t2);

    pthread_join(thread_id[1], &val);
    printf("PID = %i - Terminated all threads\n", getpid());

    return 0;
}
