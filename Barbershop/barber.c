// Eduardo Larios Fernández - A00569364
// ITESM CQ - ISC
// 24/03/2018

#include "header.h"

void a_barber(char *program)
{
	int i, k, sem_id;
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

	while (1)
	{
		printf("Barber number: %i is attending client number: %i\n", getpid(), semctl(sem_id, CLIENTS, GETVAL, 0));
		sem_wait(sem_id, CLIENTS, 1);
		printf("The number of chairs has been updated.\n");
		sem_wait(sem_id, SHAVING_ROOM, 1);
		printf("A chair has become available.\n");
		sem_signal(sem_id, WAITING_ROOM, 1);
		printf("Barber number: %i is ready.\n", getpid());
		sem_signal(sem_id, BARBER, 1);
		printf("The chairs are currently blocked.\n");
		sem_signal(sem_id, SHAVING_ROOM, 1);
	}
	exit(0);
}

int main(int argc, char *argv[])
{
	int pid, i;

	if (argc != 1)
	{
		fprintf(stderr, "Usage: %s\n", argv[0]);
		return -1;
	}

	if ((pid = fork()) < 0)
	{
		perror(argv[0]);
		return -1;
	}
	else if (pid == 0)
	{
		a_barber(argv[0]);
	}
	else
	{
		//do_nothing
	}

	return 0;
}
