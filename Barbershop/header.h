#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define BARBER			 0
#define CLIENTS 		 1
#define SHAVING_ROOM 2
#define WAITING_ROOM 3

int sem_wait(int semid, int semnum, unsigned int val) {
	struct sembuf op;

	op.sem_num = semnum;
	op.sem_op = -val;
	op.sem_flg = 0;
	return semop(semid, &op, 1);
}

int sem_signal(int semid, int semnum, unsigned int val) {
	struct sembuf op;

	op.sem_num = semnum;
	op.sem_op = val;
	op.sem_flg = 0;
	return semop(semid, &op, 1);
}

#endif
