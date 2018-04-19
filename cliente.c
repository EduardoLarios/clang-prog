#include "header.h"
#include <string.h>

#define BARCOS  4
#define TAMANIO 16
#define NICK    20
#define ACIERTO 1
#define FALLO   2
#define HUNDIDO 3
#define GANAR   4
#define PERDER  5

// B de barco
// H de hundido
// A de agua
// . de inexplorado

int main(int argc, char *argv[]){
	int sfd, puerto, i, posicion, quedan = BARCOS, coordenada, jugador, respuesta;
	char tablero[TAMANIO], oponente[TAMANIO], ip[15];
  struct sockaddr_in dir_serv; // 
  char * nickname;
  srand(getpid()); // Semilla.

  if (argc != 3) {
    printf("forma de uso: %s [direccion_ip] [puerto]\n", argv[0]);
    return -1;
  }

  strcpy(ip, argv[1]); // Copia a ip la dirección del servidor
  puerto = atoi(argv[2]);
  if(puerto <= 0){
    printf("%s: El puerto debe ser un número entero positivo.\n", argv[0]);
    return -1;
  }

  if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { // Crea el socket cliente, ipv4, tcp y default
    perror("socket");
    return -1;
  }

  // Se definen valores de estructura datos servidor
  dir_serv.sin_family = AF_INET;
  dir_serv.sin_addr.s_addr = inet_addr(ip);
  dir_serv.sin_port = htons(puerto);

  // Te conectas con tu sfd con los datos de un sockaddr y su tamaño
  if ( connect(sfd, (struct sockaddr *) &dir_serv, sizeof(dir_serv)) < 0 ) {
    perror("connect");
    return -1;
  }

  // *** Si llega aquí la conexión fue aceptada *** //

  // Espacio para el nick.
  nickname = (char *) malloc(NICK + 1);
  // Llena tableros con . de inexplorado
	memset(tablero, '.', TAMANIO);
	memset(oponente, '.', TAMANIO); 
	
	// Genera barcos
 	for(i=0; i<	BARCOS; i++){
 		posicion = rand() % TAMANIO;
 		if(tablero[posicion] != 'B'){
 			tablero[posicion] = 'B'; // B de Barco.	
 		}
 		else{ // Para que siempre genere 4.
 			i--;
 		}
 	}

  printf("Esperando oponente...\n"); 
  
  // Espero hasta recibir #jugador
  read(sfd, &jugador, sizeof(jugador)); // *** Recibo qué jugador soy

  printf("Jugador %d, introduce tu nickname: ", jugador);  
  scanf("%s", nickname);  
  write(sfd, &nickname, sizeof(nickname));  // *** Envío mi nick

  // Muestro tablero
  printf("Tu tablero:\n");
  for(i=0; i<TAMANIO; i++){
    printf("%c", tablero[i]);
    if((i+1)%4 == 0){
      printf("\n");
    }
  }

  printf("Oponente:\n");
  for(i=0; i<TAMANIO; i++){
    printf("%c", oponente[i]);
    if((i+1)%4 == 0){
      printf("\n");
    }
  }

  if(jugador == 1){ // Si eres jugador 1
    while(1){
      // Jugada Tirar
      printf("Introduce una coordenada [0..15]: ");
      scanf("%d", &coordenada);
      write(sfd, &coordenada, sizeof(coordenada)); // *** Envía coordenada
      read(sfd, &respuesta, sizeof(respuesta)); // *** Recibo si hundí o no
      if(respuesta == ACIERTO){ // Si acerté
        printf("Hundiste barco! :)\n");
        oponente[coordenada] = 'H';
      } else if(respuesta == FALLO) { 
        printf("Tocaste agua :(\n");
        oponente[coordenada] = 'A';
      } else if(respuesta == HUNDIDO) { 
        printf("Ya habías hundido este barco :(\n");
      } else if(respuesta == PERDER) { 
        printf("Ganaste el juego :)!\n");
        close(sfd);
        exit(0);
      }

      printf("Tu tablero:\n");
      for(i=0; i<TAMANIO; i++){
        printf("%c", tablero[i]);
        if((i+1)%4 == 0){
          printf("\n");
        }
      }

      printf("Oponente:\n");
      for(i=0; i<TAMANIO; i++){
        printf("%c", oponente[i]);
        if((i+1)%4 == 0){
          printf("\n");
        }
      }

      // Jugada recibir tiro
      printf("Recibiendo jugada oponente...");
      read(sfd, &coordenada, sizeof(coordenada));  // *** Recibe coordenada
      if(tablero[coordenada] == 'B'){ // Atinaron a barco
        printf("Te hundieron barco en %d...\n", coordenada);
        tablero[coordenada] = 'H';
        quedan--;
        if(quedan == 0){
          respuesta = PERDER;
          write(sfd, &respuesta, sizeof(respuesta));
          break;
        } 
        respuesta = ACIERTO;
        write(sfd, &respuesta, sizeof(respuesta));
      } else if(tablero[coordenada] == '.') {
        printf("Tocaron agua en tu tablero en %d...\n", coordenada);
        respuesta = FALLO;
        write(sfd, &respuesta, sizeof(respuesta));
      } else if(tablero[coordenada] == 'H'){
        printf("Ya habían hundido un barco en tu tablero en %d...\n", coordenada); 
        respuesta = HUNDIDO;
        write(sfd, &respuesta, sizeof(respuesta));
      } else if(respuesta == PERDER) { 
        printf("Ganaste el juego :)!\n");
        close(sfd);
        exit(0);
      }

      printf("Tu tablero:\n");
      for(i=0; i<TAMANIO; i++){
        printf("%c", tablero[i]);
        if((i+1)%4 == 0){
          printf("\n");
        }
      }

      printf("Oponente:\n");
      for(i=0; i<TAMANIO; i++){
        printf("%c", oponente[i]);
        if((i+1)%4 == 0){
          printf("\n");
        }
      }
    }
    respuesta = PERDER;
    write(sfd, &respuesta, sizeof(respuesta));
    printf("Perdiste.\n");
    close(sfd);
  } 

  else if(jugador == 2) { // Si eres jugador 2 (exactamente al revés)
    while(1) {
      // Jugada recibir tiro
      printf("Recibiendo jugada oponente...");
      read(sfd, &coordenada, sizeof(coordenada));  // *** Recibe coordenada
      if(tablero[coordenada] == 'B'){ // Atinaron a barco
        printf("Te hundieron barco en %d...\n", coordenada);
        tablero[coordenada] = 'H';
        quedan--;
        if(quedan == 0){
          respuesta = PERDER;
          write(sfd, &respuesta, sizeof(respuesta));
          break;
        } 
        respuesta = ACIERTO;
        write(sfd, &respuesta, sizeof(respuesta));
      } else if(tablero[coordenada] == '.') {
        printf("Tocaron agua en tu tablero en %d...\n", coordenada);
        respuesta = FALLO;
        write(sfd, &respuesta, sizeof(respuesta));
      } else if(tablero[coordenada] == 'H'){
        printf("Ya habían hundido un barco en tu tablero en %d...\n", coordenada); 
        respuesta = HUNDIDO;
        write(sfd, &respuesta, sizeof(respuesta));
      } else if(respuesta == PERDER) { 
        printf("Ganaste el juego :)!\n");
        close(sfd);
        exit(0);
      }

      printf("Tu tablero:\n");
      for(i=0; i<TAMANIO; i++){
        printf("%c", tablero[i]);
        if((i+1)%4 == 0){
          printf("\n");
        }
      }

      printf("Oponente:\n");
      for(i=0; i<TAMANIO; i++){
        printf("%c", oponente[i]);
        if((i+1)%4 == 0){
          printf("\n");
        }
      }

      // Jugada Tirar
      printf("Introduce una coordenada [0..15]: ");
      scanf("%d", &coordenada);
      write(sfd, &coordenada, sizeof(coordenada)); // *** Envía coordenada
      read(sfd, &respuesta, sizeof(respuesta)); // *** Recibo si hundí o no
      if(respuesta == ACIERTO){ // Si acerté
        printf("Hundiste barco! :)\n");
        oponente[coordenada] = 'H';
      } else if(respuesta == FALLO) { 
        printf("Tocaste agua :(\n");
        oponente[coordenada] = 'A';
      } else if(respuesta == HUNDIDO) { 
        printf("Ya habías hundido este barco :(\n");
      } else if(respuesta == PERDER) { 
        printf("Ganaste el juego :)!\n");
        close(sfd);
        exit(0);
      }

      printf("Tu tablero:\n");
      for(i=0; i<TAMANIO; i++){
        printf("%c", tablero[i]);
        if((i+1)%4 == 0){
          printf("\n");
        }
      }

      printf("Oponente:\n");
      for(i=0; i<TAMANIO; i++){
        printf("%c", oponente[i]);
        if((i+1)%4 == 0){
          printf("\n");
        }
      }
    }
    respuesta = PERDER;
    write(sfd, &respuesta, sizeof(respuesta));
    printf("Perdiste.\n");
    close(sfd);
  }
  return 0;
}