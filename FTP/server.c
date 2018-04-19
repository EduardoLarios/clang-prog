#include "header.h"

char *route;
char *program;

void log_keeper(char *message)
{
    FILE *file;
    char *log_filename;

    log_filename = route;
    strcat(log_filename, "/logs/log");
    file = fopen(log_filename, "a");

    if (file == NULL)
    {
        printf("Error: Logging Error\n");
        exit(-1);
    }

    fscanf(file, "%s", message);
    fclose(file);
}

void error(int nsfd, short error, char *message)
{
    short code = error;
    long message_len = strlen(message);

    write(nsfd, &code, sizeof(code));
    write(nsfd, &message_len, sizeof(message_len));
    write(nsfd, message, message_len);

    char log_message[200];
    sprintf(log_message, "Error: %s\n", message);
    log_keeper(log_message);
}

void response(int nsfd, short codes, char *message)
{
    short code = codes;
    long message_len = strlen(message);

    write(nsfd, &code, sizeof(code));
    write(nsfd, &message_len, sizeof(message_len));
    write(nsfd, message, message_len);

    char log_message[200];
    sprintf(log_message, "Message: %s\n", message);
    log_keeper(log_message);
}

void finish(int nsfd, short code, char *message)
{
    char log_message[200];
    sprintf(log_message, "Message: %s\n", message);
    log_keeper(log_message);
}

void get_file(int nsfd, short code, char *message)
{
    long message_len;
    int fd_in, stream;
    char buffer;

    if (strstr(message, "..") != NULL)
    {
        error(nsfd, (short)DENIED, "Error: The route specified did not match any valid route");
        return;
    }

    char *file_path = malloc(sizeof(message) + 2 + sizeof(route));
    strcat(file_path, route);
    strcat(file_path, message);

    char log_message[200];
    sprintf(log_message, "File Directory: %s\n", file_path);
    log_keeper(log_message);

    if ((fd_in = open(file_path, O_RDONLY)) < 0)
    {
        error(nsfd, (short)FILE_NOT_FOUND, "Error: File not found");
        free(file_path);
        return;
    }

    struct stat path_stat;
    stat(file_path, &path_stat);

    if (S_ISREG(path_stat.st_mode) == 0)
    {
        error(nsfd, (short)IS_DIR, "Error: File not found");
        free(file_path);
        return;
    }

    code = (short)SENDING_FILE;
    char aux[100];

    response(nsfd, code, "NULL");
    read(nsfd, &code, sizeof(code));
    read(nsfd, &message_len, sizeof(message_len));
    read(nsfd, aux, message_len);

    char letter[1] = {};

    do
    {
        buffer = '\0';
        stream = read(fd_in, &buffer, sizeof(buffer));

        memset(letter, '\0', sizeof(letter));
        strcpy(letter, &buffer);
        response(nsfd, code, letter);

        if (stream == 0)
        {
            break;
        }

        read(nsfd, &code, sizeof(code));
        read(nsfd, &message_len, sizeof(message_len));
        read(nsfd, &aux, message_len);
    } while (1);

    free(file_path);
    close(fd_in);
}

void list_directory(int nsfd, short code, char *message)
{
    long message_len;
    char file[100] = {};
    char log_message[200];

    sprintf(log_message, "List Directory: %s\n", message);
    log_keeper(log_message);

    if (strstr(message, "..") != NULL)
    {
        error(nsfd, (short)DENIED, "The route specified did not match any valid route");
        return;
    }

    char *file_path = malloc(sizeof(message) + 2 + sizeof(route));
    strcat(file_path, route);
    strcat(file_path, message);

    DIR *dir;
    struct dirent *dir_entry;
    struct stat buf;

    if (strlen(message) <= 1)
    {
        memset(file_path, '\0', sizeof(file_path));
        strncpy(file_path, route, sizeof(route));
    }

    if ((dir = opendir(file_path)) == NULL)
    {
        error(nsfd, (short)D_NOT_FOUND, "Directory not found");
        free(file_path);
        return;
    }

    code = SENDING_DIR;
    response(nsfd, code, "NULL");
    read(nsfd, &code, sizeof(code));
    read(nsfd, &message_len, sizeof(message_len));
    read(nsfd, file, message_len);

    while ((dir_entry = readdir(dir)) != NULL)
    {
        if (strcmp(dir_entry->d_name, ".") != 0 && strcmp(dir_entry->d_name, "..") != 0)
        {
            memset(file, '\0', sizeof(file));
            sprintf(file, "%s/%s", file_path, dir_entry->d_name);
            response(nsfd, code, file);
            read(nsfd, &code, sizeof(code));
            read(nsfd, &message_len, sizeof(message_len));
            read(nsfd, &file, message_len);
        }
    }

    response(nsfd, code, "NULL");
    free(file_path);
    closedir(dir);
}

void *server_client(void *param)
{
    int nsfd = *((int *)param);
    short code = (short)WELCOME;
    long message_len;

    response(nsfd, code, "Opening FTP Session");

    do
    {
        read(nsfd, &code, sizeof(code));
        read(nsfd, &message_len, sizeof(message_len));
        char *route = malloc(message_len);
        read(nsfd, route, message_len);

        switch (code)
        {
        case (short)FILE_ROUTE:
            get_file(nsfd, code, route);
            break;
        case (short)DIRECTORY_ROUTE:
            list_directory(nsfd, code, route);
            break;
        case (short)END:
            finish(nsfd, code, route);
            break;
        default:
            code = (short)CMD_UNKNOWN;
            response(nsfd, code, "Invalid Command");
            break;
        }
        free(route);
    } while (code != (short)END);

    close(nsfd);
}

void server(char *ip_address, int port)
{
    int socket_fd, nsfd, pid;
    pthread_t thread_id;
    char log_message[200];
    struct sockaddr_in server_info, client_info;
    DIR *dir;

    if ((dir = opendir(route)) == NULL)
    {
        perror(program);
        exit(-1);
    }

    // Initialize socket file descriptor
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror(program);
        exit(-1);
    }

    server_info.sin_family = AF_INET;
    server_info.sin_addr.s_addr = inet_addr(ip_address);
    server_info.sin_port = htons(port);

    if (bind(socket_fd, (struct sockaddr *)&server_info, sizeof(server_info)) < 0)
    {
        perror(program);
        exit(-1);
    }

    listen(socket_fd, 1);

    while (1)
    {
        int len = sizeof(client_info);

        if ((nsfd = accept(socket_fd, (struct sockaddr *)&client_info, &len)) < 0)
        {
            perror(program);
            exit(-1);
        }

        time_t tmp = time(NULL);
        struct tm local_time = *localtime(&tmp);

        sprintf(log_message, "Client: %i.%i.%i.%i date: %d-%d-%d %d:%d:%d \n",
                (int)(client_info.sin_addr.s_addr & 0xFF),
                (int)((client_info.sin_addr.s_addr & 0xFF00) >> 8),
                (int)((client_info.sin_addr.s_addr & 0xFF0000) >> 16),
                (int)((client_info.sin_addr.s_addr & 0xFF000000) >> 24),
                local_time.tm_year + 1900, 
                local_time.tm_mon + 1,
                local_time.tm_mday,
                local_time.tm_hour,
                local_time.tm_min,
                local_time.tm_sec);

        log_keeper(log_message);

        pthread_create(&thread_id, NULL, server_client, ((void *)&nsfd));
    }
}

int main(int argc, char *argv[])
{
    char ip_address[16];
    int port;

    strcpy(ip_address, DEFAULT_IP);
    port = DEFAULT_PORT;

    if (argc != 2)
    {
        printf("Usage: %s [dir]\n", argv[0]);
        return -1;
    }

    program = argv[0];
    route = argv[1];
    server(ip_address, port);

    return 0;
}
