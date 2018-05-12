// Eduardo Larios Fernandez
// A00569364 - ISC
// Programacion Avanzada
// 18 de Abril de 2018

#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "semaphore.h"
#include <time.h>

#define BUFFER_MAX 2048

#define CONECTION_START 1
#define SEND_FILE 101
#define LIST_DIRECTORY 102
#define CLOSE_CONECTION 103

#define PERMISSION_DENIED 201
#define FILE_NOT_FOUND 202
#define INTERNAL_ERROR 203
#define UNKNOWN_COMMAND 204
#define PATH_IS_DIR 205
#define SENDING_FILE 301
#define DIRECTORY_NOT_FOUND 206
#define NOT_A_DIRECTORY 207
#define LISTING_DIR_CONTENT 302

// Header for server-client communication
typedef struct message
{
    int length;
    short int code;
} message;

int send_message(int sfd, struct message *msg, short int code, char *data)
{
    size_t length = strlen(data) + 1;
    msg->length = length;
    msg->code = code;

    write(sfd, msg, sizeof(struct message));
    write(sfd, data, msg->length);

    return 0;
}

int read_message(int sfd, struct message *msg, char *data)
{
    read(sfd, msg, sizeof(struct message));
    if (msg->length > BUFFER_MAX)
    {
        return -1;
    }
    read(sfd, data, msg->length);

    return 0;
}

#endif
