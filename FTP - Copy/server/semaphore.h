// Eduardo Larios Fernandez
// A00569364 - ISC
// Programacion Avanzada
// 18 de Abril de 2018

#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define MUTEX 0
#define KEY 13

int mutex_wait(int sem_id, int sem_num)
{
    struct sembuf op;

    op.sem_num = sem_num;
    op.sem_op = -1;
    op.sem_flg = 0;
    semop(sem_id, &op, 1);
}

int mutex_signal(int sem_id, int sem_num)
{
    struct sembuf op;

    op.sem_num = sem_num;
    op.sem_op = 1;
    op.sem_flg = 0;
    semop(sem_id, &op, 1);
}

int sem_wait(int sem_id, int sem_num, int val)
{
    struct sembuf op;

    op.sem_num = sem_num;
    op.sem_op = -val;
    op.sem_flg = 0;
    semop(sem_id, &op, 1);
}

int sem_signal(int sem_id, int sem_num, int val)
{
    struct sembuf op;

    op.sem_num = sem_num;
    op.sem_op = val;
    op.sem_flg = 0;
    semop(sem_id, &op, 1);
}

#endif
