// Eduardo Larios Fernández - A00569364
// ITESM CQ - ISC
// 24/03/2018

#include "header.h"

void a_customer(char *program, int return_time)
{
	int i, sem_id;
	key_t key;

	if ((key = ftok("/dev/null", 125)) == (key_t)-1)
	{
		perror(program);
		exit(-1);
	}

	if ((sem_id = semget(key, 4, 0666)) < 0)
	{
		perror(program);
		exit(-1);
	}

	for (i = 0; i < 4; i++)
	{
		printf("Client number: %i is trying to access the barber.\n", getpid());
		sem_wait(sem_id, SHAVING_ROOM, 1);
		if (semctl(sem_id, WAITING_ROOM, GETVAL, 0) > 0)
		{
			printf("Client number: %i is accesing the barber.\n", getpid());
			sem_signal(sem_id, WAITING_ROOM, 1);
			printf("Client number: %i is in the waiting room.\n", getpid());
			sem_signal(sem_id, CLIENTS, 1);
			printf("Client number: %i is in the shaving room.\n", getpid());
			sem_signal(sem_id, SHAVING_ROOM, 1);
			sem_wait(sem_id, BARBER, 1);
			printf("Client number: %i is going to sleep.\n", getpid());
			sleep(return_time);
		}
		else
		{

			printf("The Barbershop is FULL.\n");
			sem_signal(sem_id, SHAVING_ROOM, 1);
			printf("Client number %i is going to sleep.\n", getpid());
			sleep(return_time);
		}
	}
	exit(0);
}

int main(int argc, char *argv[])
{
	int clients = 5;
	int pid, i, return_time;

	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s [client_return_time]\n", argv[0]);
		return -1;
	}

	return_time = atoi(argv[1]);
	if (return_time < 1)
	{
		fprintf(stderr, "%s: The wait time must be a positive integer greater than zero.\n", argv[0]);
		return -1;
	}

	for (i = 0; i < clients; i++)
	{
		if ((pid = fork()) < 0)
		{
			perror(argv[0]);
			return -1;
		}
		else if (pid == 0)
		{
			a_customer(argv[0], return_time);
		}
		else
		{
			//do_nothing
		}
	}

	return 0;
}
