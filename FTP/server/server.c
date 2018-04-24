// Eduardo Larios Fernandez
// A00569364 - ISC
// Programacion Avanzada
// 18 de Abril de 2018

#include "header.h"

int log_keeper(char *command, char *ip_address, int log_fd, short int code, char *message)
{
    char log_entry[BUFFER_MAX], *local_time;
    int sem_id;
    key_t key_sem;
    time_t tm;

    // Datetime formatter
    time(&tm);
    local_time = ctime(&tm);
    local_time[strlen(local_time) - 1] = '\0';

    // Log entry to be added to the log
    sprintf(log_entry, "%s\t%s\t%i\t%s: %s\n", local_time, ip_address, code, command, message);

    // Accesses semaphore mutex
    if ((key_sem = ftok("/dev/null", KEY)) == (key_t)-1)
    {
        perror("Log Keeper Error: ftok");
        return -1;
    }
    if ((sem_id = semget(key_sem, 1, 0666)) < 0)
    {
        perror("Log Keeper Error: semget");
        return -1;
    }

    // Waits until log is available to write
    mutex_wait(sem_id, MUTEX);
    if (write(log_fd, log_entry, strlen(log_entry)) < 0)
    {
        perror("Log Keeper Error: Mutex");
        mutex_signal(sem_id, MUTEX);
        return -1;
    }
    mutex_signal(sem_id, MUTEX);
    return 0;
}

int list_directory(int sfd, struct message *message, char *route, char *ip_address, int log_fd)
{
    int pid, fd[2];
    char response[BUFFER_MAX], abs_path[BUFFER_MAX + 7];

    // Validates input route
    if (strstr(route, "..") != NULL)
    {
        send_message(sfd, message, PERMISSION_DENIED, "Error: The route contains \"..\", which is not permitted");
        log_keeper("Response", ip_address, log_fd, PERMISSION_DENIED, "Error: The route contains \"..\", which is not permitted");
        return -1;
    }

    // Builds an absolute path to the root.
    sprintf(abs_path, "./root/%s/", route);

    // Pipes ls output to the client.
    if (pipe(fd) < 0)
    {
        send_message(sfd, message, INTERNAL_ERROR, "Error: Internal server error.");
        log_keeper("Response", ip_address, log_fd, INTERNAL_ERROR, "Error: Internal server error.");
        return -1;
    }
    if ((pid = fork()) < 0)
    {
        send_message(sfd, message, INTERNAL_ERROR, "Error: Internal server error.");
        log_keeper("Response", ip_address, log_fd, INTERNAL_ERROR, "Error: Internal server error.");
        return -1;
    }
    else if (pid == 0)
    {
        close(fd[0]);
        dup2(fd[1], 1);
        dup2(fd[1], 2);
        execlp("ls", "ls", "-l", abs_path, (char *)0);
        printf("a\n");
    }
    else
    {
        wait(0);
        close(fd[1]);
        dup2(fd[0], 0);

        int size = read(fd[0], response, sizeof(response));

        response[size] = '\0';
        close(fd[0]);

        if (strstr(response, "ls:") == (char *)&response)
        { 
            send_message(sfd, message, DIRECTORY_NOT_FOUND, "Error: Invalid directory.");
            log_keeper("Response", ip_address, log_fd, DIRECTORY_NOT_FOUND, "Error: Invalid directory.");
        }
        else
        {
            send_message(sfd, message, LISTING_DIR_CONTENT, response);
            log_keeper("Response", ip_address, log_fd, LISTING_DIR_CONTENT, abs_path);
        }
    }

    return 0;
}

int send_file(int sfd, struct message *message, char *route, char *ip_address, int log_fd)
{
    int pid, fd[2];
    char response[BUFFER_MAX], abs_path[BUFFER_MAX + 7];

    // Validates route
    if (strstr(route, "..") != NULL)
    {
        send_message(sfd, message, PERMISSION_DENIED, "Error: The route contains \"..\", which is not permitted.");
        log_keeper("Response", ip_address, log_fd, PERMISSION_DENIED, "Error: The route contains \"..\", which is not permitted.");
        return -1;
    }

    // Creates an absolute root path
    sprintf(abs_path, "./root/%s", route);

    // Pipes the cat output to the client
    if (pipe(fd) < 0)
    {
        send_message(sfd, message, INTERNAL_ERROR, "Error: Internal server error.");
        log_keeper("Response", ip_address, log_fd, INTERNAL_ERROR, "Error: Internal server error.");
        return -1;
    }
    if ((pid = fork()) < 0)
    {
        send_message(sfd, message, INTERNAL_ERROR, "Error: Internal server error.");
        log_keeper("Response", ip_address, log_fd, INTERNAL_ERROR, "Error: Internal server error.");
        return -1;
    }
    else if (pid == 0)
    {
        close(fd[0]);
        dup2(fd[1], 1);
        dup2(fd[1], 2);
        execlp("cat", "cat", abs_path, (char *)0);
    }
    else
    {
        wait(0);
        close(fd[1]);
        dup2(fd[0], 0);

        int size = read(fd[0], response, sizeof(response));
        response[size] = '\0';

        close(fd[0]);
        if (strstr(response, "cat:") == (char *)&response)
        { 
            send_message(sfd, message, FILE_NOT_FOUND, "Error: Invalid file.");
            log_keeper("Response", ip_address, log_fd, FILE_NOT_FOUND, "Error: Invalid file.");
        }
        else
        {
            send_message(sfd, message, SENDING_FILE, response);
            log_keeper("Response", ip_address, log_fd, SENDING_FILE, abs_path);
        }
    }

    return 0;
}

int client_handler(int nsfd, char *ip_address, int log_fd)
{
    char data_sent[BUFFER_MAX], data_received[BUFFER_MAX];
    struct message msg_sent, msg_received;

    // Accepts the client connections
    if (send_message(nsfd, &msg_sent, CONECTION_START, "FTP Session Started") < 0)
    {
        perror("Error: FTP Connection");
        return -1;
    }
    else
    {
        log_keeper("FTP Session Started", ip_address, log_fd, CONECTION_START, "Welcome");
    }

    do
    {
        // Evaluates client response
        if (read_message(nsfd, &msg_received, data_received) < 0)
        {
            send_message(nsfd, &msg_sent, INTERNAL_ERROR, "Error: Internal Error.");
            log_keeper("Response", ip_address, log_fd, INTERNAL_ERROR, "Error: Internal Error.");
            return -1;
        }
        else
        {
            log_keeper("Command", ip_address, log_fd, msg_received.code, data_received);
        }
        switch (msg_received.code)
        {
        case SEND_FILE:
            send_file(nsfd, &msg_sent, data_received, ip_address, log_fd);
            break;
        case LIST_DIRECTORY:
            list_directory(nsfd, &msg_sent, data_received, ip_address, log_fd);
            break;
        case CLOSE_CONECTION:
            break;
        default:
            send_message(nsfd, &msg_sent, UNKNOWN_COMMAND, "Error: Unknown command.");
            log_keeper("Response", ip_address, log_fd, UNKNOWN_COMMAND, "Error: Unknown command.");
        }
    } while (msg_received.code != CLOSE_CONECTION);

    log_keeper("Session Closed", ip_address, log_fd, CLOSE_CONECTION, "Goodbye");
    return 0;
}

int main(int argc, char *argv[])
{
    char ip_address[16];
    int port, sfd, nsfd, len, pid, log_fd, sem_id;
    struct sockaddr_in server_address, client_address;
    key_t key_sem;

    // Validates input arguments
    if (argc > 3)
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

        port = atoi(argv[2]);
        strcpy(ip_address, argv[3]);
        
        if(!argv[2] || !argv[3])
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

    // Socket initialization.

    if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror(argv[0]);
        return -1;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(ip_address);
    server_address.sin_port = htons(port);

    if (bind(sfd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror(argv[0]);
        close(sfd);
        return -1;
    }

    // Creates or opens the log file
    if ((log_fd = open("log", O_CREAT | O_WRONLY | O_APPEND, 0666)) < 0)
    {
        perror(argv[0]);
        close(sfd);
        return -1;
    }
    // Creates the semaphore mutex
    if ((key_sem = ftok("/dev/null", KEY)) == (key_t)-1)
    {
        perror("Error: ftok");
        close(sfd);
        close(log_fd);
        return -1;
    }
    if ((sem_id = semget(key_sem, 1, 0666 | IPC_CREAT)) < 0)
    {
        perror("Error: semget");
        close(sfd);
        close(log_fd);
        return -1;
    }
    semctl(sem_id, MUTEX, SETVAL, 1);

    // Listens for client connections
    while (1)
    {
        listen(sfd, 64);

        len = sizeof(client_address);
        if ((nsfd = accept(sfd, (struct sockaddr *)&client_address, &len)) < 0)
        {
            perror("Error: Connection Rejected");
            return -1;
        }

        if ((pid = fork()) < 0)
        {
            perror("Error: fork");
        }
        else if (pid == 0)
        {
            client_handler(nsfd, ip_address, log_fd);
        }
    }

    close(log_fd);
    close(sfd);
    return 0;
}
