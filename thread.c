#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <wait.h>
#include <unistd.h>

// A normal C function that is executed as a thread
// when its name is specified in pthread_create()
void *thread_sum(void *arg)
{
    // Castear argumento de regreso al tipo original
    int num = *((int *)arg);

    sleep(1);
    printf("Printing sum:\n");
    printf("%i\n", num);
    return NULL;
}

int main()
{
    // Inicialización de la estructura thread
    pthread_t tid;
    int num = 5;

    printf("Before Thread\n");
    // Creación del thread propiamente
    // Casteas el argumento a void pointer (void *) para poder pasarlo
    pthread_create(&tid, NULL, thread_sum, (void *)&num);
    
    // The pthread_join() function for threads is the equivalent of 
    // wait() for processes. A call to pthread_join blocks the calling 
    // thread until the thread with identifier equal to the first argument 
    // terminates. 
    pthread_join(tid, NULL);
    printf("\nAfter Thread\n");
    exit(0);
}