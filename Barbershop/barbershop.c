// Eduardo Larios Fernández - A00569364
// ITESM CQ - ISC
// 24/03/2018

#include "header.h"

int main(int argc, char *argv[])
{
	int i, sem_id, chairs;
	unsigned short final_values[4];
	key_t key;

	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s [number_of_chairs]\n", argv[0]);
		return -1;
	}

	chairs = atoi(argv[1]);
	if (chairs < 1)
	{
		fprintf(stderr, "%s: The number of chairs must be a positive integer greater than zero.\n", argv[0]);
		return -1;
	}

	if ((key = ftok("/dev/null", 125)) == (key_t)-1)
	{
		perror(argv[0]);
		return -1;
	}

	if ((sem_id = semget(key, 4, 0666 | IPC_CREAT)) < 0)
	{
		perror(argv[0]);
		return -1;
	}

	semctl(sem_id, BARBER, SETVAL, 0);
	semctl(sem_id, CLIENTS, SETVAL, 0);
	semctl(sem_id, SHAVING_ROOM, SETVAL, 1);
	semctl(sem_id, WAITING_ROOM, SETVAL, chairs);

	semctl(sem_id, BARBER, GETALL, final_values);
	printf("Value: ");
	for (i = 0; i < 4; i++)
	{
		printf("%3i", final_values[i]);
	}
	printf("\n");
	return 0;
}
