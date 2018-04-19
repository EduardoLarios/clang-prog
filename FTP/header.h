#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>

#define DEFAULT_PORT 9999
#define DEFAULT_IP "127.0.0.1"

#define LIST "list"
#define GET "get"
#define BYE "bye"

#define WELCOME 1
#define FILE_ROUTE 101
#define DIRECTORY_ROUTE 102
#define END 103
#define DENIED 201
#define FILE_NOT_FOUND 202
#define INTERNAL_ERROR 203
#define CMD_UNKNOWN 204
#define IS_DIR 205
#define D_NOT_FOUND 206
#define IS_NOT_DIR 207
#define SENDING_FILE 301
#define SENDING_DIR 302

#endif
