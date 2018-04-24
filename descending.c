// Eduardo Larios Fernández
// A00569364 - ISC
// Programación Avanazada
// 26/Feb/2018

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

// Declaración previa como prototipo de la función
void create_child(int i, int num_lvl, char* program);

// Función que crea los procesos requeridos por el programa
void create_parent(int i, int num_lvl, char* program) 
{
    // Obtiene el valor de retorno de fork
	int pid = fork();
	int j;
	
	// Si pid es menor a 0 entonces regresa un valor de error al crear el proceso
	if (pid < 0) 
	{
		perror(program);
		exit(-1);
		
	}
	// Vuelve el control al proceso hijo
	else if (pid == 0) 
	{
		create_child(++i, num_lvl, program);
	}
	// Si pid es mayor a 0 entonces obtiene el valor PID del proceso hijo
	else 
	{
		for (j = 0; j < i; j++) 
		{
			printf("\t");
		}
		
		// Imprime los detalles del proceso obtenido con PID
		printf("PPID = %i PID = %i LEVEL = %i\n", getppid(), getpid(), i);
		// Espera a terminar el proceso y luego sale de su ejecución
		wait(NULL);
		exit(0);
	}
}

// Función que crea un proceso hijo a partir de un proceso padre
void create_child(int i, int num_lvl, char* program) {
    int pid, j;
	
	// Si el proceso regresa 0 entonces crea un nuevo proceso
	if (i == 0) 
	{
		create_parent(i, num_lvl, program);
	} 
	// En caso de que el iterador sea diferente a la altura requerida (el nivel + 1)
	// Continua creado más procesos
	else if (i != (num_lvl + 1)) 
	{
		for (j = 0; j < i; j++) 
		{
			create_parent(i, num_lvl, program);
		}
	}
}

int main(int argc, char *argv[])
{
	// Recibe los argumentos del programa
	int num_lvl;
	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s [level]\n", argv[0]);
		return -1;
	}
    
	num_lvl = atoi(argv[1]);

	// Verifica que el argumento de número de procesos sea válido
	if (num_lvl <= 0)
	{
		fprintf(stderr, "Error: level must be a positive integer greater than 0\n");
		return -1;
	}
	
	
    // Llama la función que crea el número requerido de procesos
	create_parent(0, num_lvl, argv[0]);

	return 0;
}