#include "header.h"
#include <string.h>

#define NICK    20
#define ACIERTO 1
#define FALLO 	2
#define HUNDIDO 3
#define GANAR	4
#define PERDER 	5


void bark(int j1fd, int j2fd){
	// Si llegamos aquí es por que ya están conectados 2 jugadores.
	int j1 = 1, j2 = 2, win = 0, coordenada=0, respuesta=0, flg=0;
	char * nick1; nick1 = (char *) malloc(NICK + 1);
	char * nick2; nick2 = (char *) malloc(NICK + 1);

	write(j1fd, &j1, sizeof(j1));
	write(j2fd, &j2, sizeof(j2));
	read(j1fd, nick1, sizeof(nick1));
	read(j2fd, nick2, sizeof(nick2));

	while(1){
		// Tiro J1
		read(j1fd, &coordenada, sizeof(coordenada)); // Leemos coordenada enviada por J1
	 	printf("Leo coordenada J1: %d\n", coordenada);
		write(j2fd, &coordenada, sizeof(coordenada)); // Envío coordenada a J2
		read(j2fd, &respuesta, sizeof(respuesta)); // Recibo respuesta de J2
		printf("Leo  respuesta J1: %d\n", respuesta);
		write(j1fd, &respuesta, sizeof(respuesta)); // Envío respuesta a J1

		if(respuesta == PERDER){
			printf("Jugador 2 pierde. \n");
			exit(0);
		}

		// Tiro J2
		read(j2fd, &coordenada, sizeof(coordenada)); // Leemos coordenada enviada por J2
	 	printf("Leo coordenada J2: %d\n", coordenada);
		write(j1fd, &coordenada, sizeof(coordenada)); // Envío coordenada a J1
		read(j1fd, &respuesta, sizeof(respuesta)); // Recibo respuesta de J1
		printf("Leo  respuesta J2: %d\n", respuesta);
		write(j2fd, &respuesta, sizeof(respuesta)); // Envío respuesta a J2
		
		if(respuesta == PERDER){
			printf("Jugador 1 pierde. \n");
			exit(0);
		}
	}
	exit(0);
}


int main(int argc, char *argv[]){
	int sfd, puerto, j1fd, j2fd, pid, s1, s2;
	struct sockaddr_in servidor, dj1, dj2;
	char ip[15];
	
	if (argc > 3) {
    printf("forma de uso: %s [direccion_ip] [puerto]\n", argv[0]);
    return -1;
  }

  strcpy(ip, argv[1]); // Copia a ip la dirección del servidor
  puerto = atoi(argv[2]);
  if(puerto <= 0){
    printf("%s: El puerto debe ser un número entero positivo.\n", argv[0]);
    return -1;
  }

  if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) { // crear sfd de servidor
		perror("socket");
		return -1;
	}
	// Asigna valores al sockaddr del servidor
	servidor.sin_family = AF_INET;
	servidor.sin_addr.s_addr = inet_addr(ip);
	servidor.sin_port = htons(puerto);

	// Asociamos sfd con el sockaddr
	if (bind(sfd, (struct sockaddr *) &servidor, sizeof(servidor)) == -1) {
		perror("bind");
		return -1;
	}

	// Escucho conexiones
	listen(sfd, 5);

	while(1){
		// Esto lo creía inútil \.____./
		s1 = sizeof(dj1);
		s2 = sizeof(dj2);
		if ((j1fd = accept(sfd, (struct sockaddr *) &dj1, &s1)) == -1) { // Reconozco a J1
			perror("accept");
			return -1;
		}
		printf("Entró jugador 1...\n");
		if ((j2fd = accept(sfd, (struct sockaddr *) &dj2, &s2)) == -1) { // Reconozco a J2
			perror("accept");
			return -1;
		}
		printf("Entró jugador 2...\n");

		if ((pid = fork()) == -1) {  // Hace un proceso hijo
			perror("fork");
			return -1;
		} else if (pid == 0) {
			close(sfd); // Pues ya tienen el accept, por ello no necesita el child el sfd
			bark(j1fd, j2fd);
			return 0;
		}
		close(sfd); close(j1fd); close(j2fd); // Cierra ya el asuntol
	}
}