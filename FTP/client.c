#include "header.h"

void error(char *e_msg)
{
    printf("Error: %s\n", e_msg);
}

void menu()
{
    printf("Message: Send File [get] [route]\n");
    printf("Message: List Directory [list] [route]\n");
    printf("Message: Send File [bye]\n");
}

void send_message(int socket_fd, short code, char *message)
{
    long message_len = sizeof(message);
    write(socket_fd, &code, sizeof(code));
    write(socket_fd, &message_len, sizeof(message_len));
    write(socket_fd, message, message_len);
}

void get_file(int socket_fd, char *message)
{
    short code;
    long message_len;
    char file[100] = {}, aux[100];
    char *response, *file_name;
    int fd_out, stream;

    send_message(socket_fd, FILE_ROUTE, message);
    read(socket_fd, &code, sizeof(code));
    read(socket_fd, &message_len, sizeof(message_len));
    read(socket_fd, response, message_len);

    switch (code)
    {
    case ((short)SENDING_FILE):
        file_name = &message[0];

        while (strstr(file_name, "/") != NULL)
        {
            file_name = &file_name[1];
        }

        strncpy(file, file_name, strlen(file_name));

        if ((fd_out = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0666)) < 0)
        {
            printf("Error: Could not write to file\n");
            return;
        }

        send_message(socket_fd, code, "NULL");

        do
        {
            memset(aux, '\0', sizeof(aux));
            read(socket_fd, &code, sizeof(code));
            read(socket_fd, &message_len, sizeof(message_len));
            read(socket_fd, &aux, message_len);

            if (aux[0] == '\0')
            {
                close(fd_out);
                break;
            }

            stream = (int)message_len;
            write(fd_out, &aux[0], sizeof(aux[0]));
            send_message(socket_fd, code, "NULL");

        } while (1);

        return;
        break;

    case ((short)DENIED):
        error(response);
        return;
        break;
    case ((short)FILE_NOT_FOUND):
        error(response);
        return;
        break;
    case ((short)CMD_UNKNOWN):
        error(response);
        return;
        break;
    }
}

void list_directory(int socket_fd, char *message)
{
    short code;
    long message_len;
    char *response;
    char aux[100];

    send_message(socket_fd, DIRECTORY_ROUTE, message);
    read(socket_fd, &code, sizeof(code));
    read(socket_fd, &message_len, sizeof(message_len));
    read(socket_fd, response, message_len);

    switch (code)
    {
    case (short)SENDING_DIR:
        send_message(socket_fd, code, "NULL");

        do
        {
            memset(aux, '\0', sizeof(aux));
            read(socket_fd, &code, sizeof(code));
            read(socket_fd, &message_len, sizeof(message_len));
            read(socket_fd, &aux, message_len);

            if (strcmp(aux, "NULL") == 0)
            {
                break;
            }

            printf("%s\n", aux);
            send_message(socket_fd, code, "NULL");

        } while (1);

        break;

    case (short)DENIED:
        error(response);
        break;
    case (short)D_NOT_FOUND:
        error(response);
        break;
    case (short)CMD_UNKNOWN:
        error(response);
        break;
    }
}

void client_server(int socket_fd)
{
    short code;
    long message_len;
    char option[NAME_MAX + 1], split[NAME_MAX + 1], command[4];
    int i = 0, j = 0;
    char *route;

    read(socket_fd, &code, sizeof(code));
    read(socket_fd, &message_len, sizeof(message_len));

    char *message = malloc(sizeof(message_len) + 1);
    read(socket_fd, message, message_len);
    printf("%s\n", message);

    do
    {
        menu();
        memset(split, 0, sizeof(split));
        memset(option, 0, sizeof(option));
        memset(command, 0, sizeof(command));

        printf("Command: ");
        fgets(option, sizeof(option), stdin);

        while (option[i] != ' ')
        {
            split[i] = option[i];
            i++;
        }

        i++;
        strcpy(command, split);

        if (strcmp(command, BYE) != 0)
        {
            memset(split, 0, sizeof(split));

            while (option[i] != '\0')
            {
                split[j] = option[i];
                i++;
                j++;
            }

            route = split;
        }

        if (strcmp(command, GET) == 0)
        {
            if (strlen(route) <= 0)
            {
                printf("Error: Invalid command\n");
                printf("Usage: [get] [route]\n");
            }
            else
            {
                get_file(socket_fd, route);
            }
        }

        if (strcmp(command, LIST) == 0)
        {
            if (strlen(route) <= 0)
            {
                printf("Error: Invalid command\n");
                printf("Usage: [get] [route]\n");
            }
            else
            {
                list_directory(socket_fd, route);
            }
        }

        if (strcmp(command, BYE) == 0)
        {
            send_message(socket_fd, END, "Ending FTP Session");
            code = (short)END;
        }
    } while (code != (short)END);
}

int main(int argc, char *argv[])
{
    int socket_fd, port;
    struct sockaddr_in server_info;

    if (argc != 3)
    {
        printf("Usage: %s [ip_address] [port]\n", argv[0]);
        return -1;
    }

    port = atoi(argv[2]);

    if (port < 5000)
    {
        printf("%s: The port must be greater than 5000.\n", argv[0]);
        return -1;
    }

    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror(argv[0]);
        return -1;
    }

    server_info.sin_family = AF_INET;
    server_info.sin_addr.s_addr = inet_addr(argv[1]);
    server_info.sin_port = htons(port);

    if (connect(socket_fd, (struct sockaddr *)&server_info, sizeof(server_info)) < 0)
    {
        perror(argv[0]);
        return -1;
    }

    client_server(socket_fd);

    close(socket_fd);
    return 0;
}
