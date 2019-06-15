// Eduardo Larios Fernández
// A00569364 - ISC
// 09/Abril/2018
// Compile with: gcc root.c -o root -lpthread -lm

#include <stdlib.h>
#include <sys/time.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>

#define SIZE 1000

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t people_garden = PTHREAD_COND_INITIALIZER;

long int people_num = 0;

void check_garden()
{
    int i, random1, random2;
    for (i = 0; i < 100; i++)
    {
        pthread_mutex_lock(&mutex);
        if (people_num == SIZE)
        {
            pthread_cond_wait(&people_garden, &mutex);
        }

        random1 = (rand() % 30);
        people_num += random1;
        pthread_mutex_unlock(&mutex);
        random2 = (rand() % 20);

        if (people_num && people_num > random2)
        {
            pthread_mutex_lock(&mutex);
            people_num -= random2;
            pthread_cond_signal(&people_garden);
            pthread_mutex_unlock(&mutex);
        }
        printf("Number of people in the garden %li...\n", people_num);
    }
}

void *east(void *arg)
{
    printf("East entrance opened...\n");
    check_garden();
    pthread_exit(NULL);
}

void *west(void *arg)
{
    printf("West entrance opened...\n");
    check_garden();
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    time_t t;
    pthread_t west_thread;
    pthread_t east_thread;
    srand((unsigned)time(&t));

    pthread_create(&west_thread, NULL, west, NULL);
    pthread_create(&east_thread, NULL, east, NULL);
    pthread_join(west_thread, NULL);
    pthread_join(east_thread, NULL);
    printf("The garden is closed.\n");
    if (people_num)
    {
        pthread_mutex_lock(&mutex);
        people_num -= people_num;
        pthread_cond_signal(&people_garden);
        pthread_mutex_unlock(&mutex);
    }
    return 0;
}
