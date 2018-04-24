// Eduardo Larios Fernandez
// A00569364 - ISC
// Programacion Avanzada
// 18 de Abril de 2018

#include "header.h"

int main(int argc, char *argv[])
{
    int sfd, code, port, gfd;
    char *ptr_c;
    char ip_address[16], buffer[BUFFER_MAX], message_in[BUFFER_MAX], message_out[BUFFER_MAX];
    struct sockaddr_in server_address;
    struct message *message;

    // Validates input parameters.
    if (argc > 5)
    {
        printf("Usage: %s [ip_address] [port]\n", argv[0]);
        return -1;
    }
    if (argc == 1)
    {
        strcpy(ip_address, "127.0.0.1");
        port = 9999;
    }
    else if (argc == 3)
    {
        if (strcmp(argv[1], "-d") == 0)
        {
            strcpy(ip_address, argv[2]);
            port = 9999;
        }
        else if (strcmp(argv[1], "-p") == 0)
        {
            strcpy(ip_address, "127.0.0.1");
            port = atoi(argv[2]);
        }
        else
        {
            printf("Usage: %s [ip_address] [port]\n", argv[0]);
            return -1;
        }
    }
    else if (argc == 5)
    {
        if (strcmp(argv[1], "-d") == 0 && strcmp(argv[3], "-p") == 0)
        {
            strcpy(ip_address, argv[2]);
            port = atoi(argv[4]);
        }
        else
        {
            printf("Usage: %s [ip_address] [port]\n", argv[0]);
            return -1;
        }
    }
    if (port <= 5000)
    {
        printf("%s: The port must be an integer bigger than 5000\n", argv[0]);
        return -1;
    }

    // Initializes socket descriptor.

    if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror(argv[0]);
        return -1;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(ip_address);
    server_address.sin_port = htons(port);

    // Connects to the server

    if (connect(sfd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror(argv[0]);
        return -1;
    }

    // Communicates with the server

    message = (struct message *)malloc(sizeof(struct message));
    printf("Conecting to FTP server...\n");

    do
    {
        // Reads server response
        read_message(sfd, message, message_in);
        if (message->code != SENDING_FILE)
        {
            printf("%s\n", message_in);
        }
        else
        {
            if (strrchr(buffer, '/') != NULL)
                ptr_c = strrchr(buffer, '/') + 1;
            else
                ptr_c = &(buffer[strlen("get ")]);
            if ((gfd = open(ptr_c, O_CREAT | O_WRONLY, 0644)) < 0)
            {
                perror("open");
                printf("%s\n", message_in);
            }
            else
            {
                if ((write(gfd, message_in, strlen(message_in))) < 0)
                {
                    perror("write");
                    printf("%s\n", message_in);
                }
            }
        }
        if (message->code == INTERNAL_ERROR)
            break;

        // User input
        printf("\nOption: Get File [get] [route]\n");
        printf("Option: List Directory [list] [route]\n");
        printf("Option: Finish Session [bye]\n\n");
        printf("User@FTP-Server -$ ");
        fgets(buffer, BUFFER_MAX, stdin);

        ptr_c = strstr(buffer, "\n");
        *ptr_c = '\0';

        // Verifies second parameter existance
        ptr_c = strstr(buffer, " ");
        if (ptr_c == NULL)
        {
            message_out[0] = '\0';
        }
        else
        {
            ptr_c++;
            strcpy(message_out, ptr_c);
        }

        // Reads command parameter
        if (strstr(buffer, "get") == (char *)&buffer)
        {
            code = SEND_FILE;
        }
        else if (strstr(buffer, "list") == (char *)&buffer)
        {
            code = LIST_DIRECTORY;
        }
        else if (strstr(buffer, "bye") == (char *)&buffer)
        {
            printf("Clossing FTP Session\n");
            code = CLOSE_CONECTION;
        }
        else
        {
            code = 0;
        }

        // Sends message to the server
        send_message(sfd, message, code, message_out);

    } while (code != CLOSE_CONECTION);

    free(message);
    close(sfd);

    return 0;
}
