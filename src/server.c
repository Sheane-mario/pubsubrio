#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

#define MAX_CLIENTS 100

// manage globally the connected clients
typedef struct {
    int client_socket_fd;
    int is_publisher; // 1 = publisher , 0 = subscriber
} Client;

Client clients[MAX_CLIENTS];
int client_count = 0;

long PORT = 0;

void *handle_client(void *arg) {

    int client_fd = *(int *)arg;
    free(arg);

    char buff[] = "Successfully connected to the server !";
    // acknowledge the client
    int bts_send = send(client_fd, (char *)&buff, sizeof(buff), 0);
    if (bts_send < 0) {
        perror("Couldn't acknowledge the client!\n");
        exit(EXIT_FAILURE);
    }

    // storing the client connected mode and disconnecting the client if mode couldn't be configured.
    char mode[512];
    int recv_mode_s = recv(client_fd, mode, sizeof(mode) - 1, 0);
    if (recv_mode_s > 0) {
        printf("Client mode: %s \n", mode);
    } else {
        printf("Couldn't receive the client connected mode! \n");
        printf("Disconnecting client.....\n");
        close(client_fd);
        return NULL;
    }

    char client_msg[4096];
    while (1) {
        recv(client_fd, client_msg, sizeof(client_msg)-1, 0);
        if (strcmp(client_msg, "exit\n") == 0) {
            printf("Client disconnected! \n");
            close(client_fd);
            break;
        }
        printf("%s\n", client_msg); 
    }
    return NULL;

}

int main(int argc, char* argv[]) {

    // disable output buffering
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    if (argc < 2) {
        printf("Please input the server port! \n");
        return 1;
    }

    // port will be a string initially
    char* portstr;
    PORT = strtol(argv[1], &portstr, 10);

    // socket creation to use IPv4 protocol family for internet namespace and TCP
    int server_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        printf("Socket creation failed: %s \n", strerror(errno)); 
        return 1;
    }

    // set socket options
    // set SO_REUSEADDR since even after closing the socket it may keep it open for a while, so if our application is tested quite often we will get address already in use error. SO_REUSEADDR allows it to reuse.
    // SOL_SOCKET set the above option to the entire socket level, not TCP or IP.
    int reuse = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        printf("SO_REUSEADDR failed: %s \n", strerror(errno));
        return 1;
    }

    // tell the kernel where the server socket is listening on. mention the adderes family(IPv4), port number and the IPv4 address
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT); // convert from little endian to big endian since TCP/IP expect big-end
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    // bind the socket with the IP address specifiend aboce.
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        printf("Server socket bind failed: %s \n", strerror(errno));
        return 1;
    }

    int connection_backlog = 20;
    if (listen(server_fd, connection_backlog) == -1) {
        printf("Listen failed in the server: %s \n", strerror(errno));
        return 1;
    }

    //  server is waiting for the client
    printf("Waiting for a client to connect.....\n");

    // place where the client socket info will be saved
    struct sockaddr_in client_addr;
    unsigned int client_addr_len = sizeof(client_addr);

    while (1) {
        // accepting the client request 
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_fd == -1) {
            printf("Client connection unsuccessful! %s \n", strerror(errno));
            return 1; 
        }

        printf("Client connected.... \n");

        // creating child process
        //pid_t pid = fork();

        // child process
        //if (pid == 0) {
        //    close(server_fd); // child doesn't need server sock info
        //    handle_client(client_fd);
        //    exit(0);
        //} else if (pid > 0) {
            // parent process
            // parent process doesn't need child sock info
        //    close(client_fd);
        //} else {
        //    printf("Fork failed %s \n", strerror(errno));
        //    return 1;
        //}

        // had a problem when using fork() because didn't be able to get access to shared memoty and IPC was horrible, so moved to pthread optioon

        pthread_t tid;
        int* pclient = malloc(sizeof(int));
        *pclient = client_fd;

        if (pthread_create(&tid, NULL, handle_client, pclient) != 0) {
            perror("Failed to create thead! \n");
            close(client_fd);
        }

        pthread_detach(tid);
    }



    return 0;

}
