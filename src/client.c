#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

long SERVER_PORT = 0;
char* SERVER_IP;
char* MODE;

// I made client_fd global, so that multiple thread can use this socket to concurrent communication
int client_fd;

void *recv_thread_func() {
    // to store the message send from the pubs
    char publisher_msg[4096];
    while (1) {
        // receive the message from the server where publishers have sent 
        recv(client_fd, publisher_msg, sizeof(publisher_msg) - 1, 0);
        printf("%s \n", publisher_msg);
    }
    return NULL;
}

void *send_thread_func() {
    char client_command[4096];
    while (1) {
        fgets(client_command, sizeof(client_command), stdin);
        int comm_s = send(client_fd, (char *)&client_command, sizeof(client_command), 0);
        if (strcmp(client_command, "exit\n" ) == 0) {
            // here i got a bug, when the exit is typed from the send thread, it closes the socket, so that receive thead runs infinitely, which was found to be a classic multithreading bug. 
            //close(client_fd);
            // using this shutdown i send signals to both the thereads that this is going to terminate, so close the socket from their threads as well
            shutdown(client_fd, SHUT_RDWR);
            close(client_fd);
            exit(0);
        } 
        if (comm_s < 0) {
            perror("Couldn't send the message to the server! \n");
            exit(EXIT_FAILURE);
        }
    }
    return NULL;
}

int main(int argc, char* argv[]) {

    // disable output buffering
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    if (argc < 4) {
        printf("Please provide the [SERVER_IP], [SERVER_PORT] and [MODE] !\n");
        return 1;
    }

    // port will be a string initially
    char* portstr;
    SERVER_PORT = strtol(argv[2], &portstr, 10);
    SERVER_IP = argv[1];
    MODE = argv[3];

    if ((strcmp(MODE, "PUBLISHER") != 0) && (strcmp(MODE, "SUBSCRIBER") != 0)) {
        printf("Invalid Client mode ! \n");
        return 1;
    }


    client_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (client_fd == -1) {
        printf("Client Socket creation failed: %s \n", strerror(errno)); 
        return 1;
    }
    int reuse = 1;
    if (setsockopt(client_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        printf("SO_REUSEADDR failed: %s \n", strerror(errno));
        return 1;
    }

    // stores the server socket info to connect
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        printf("Couldn't connect to the server: %s \n", strerror(errno));
        return 1;
    }
    // store the message recv from the server
    char conn_succ[1024];
    // receive the message
    int bytes_rec = recv(client_fd, conn_succ, sizeof(conn_succ) - 1, 0);
    if (bytes_rec > 0) {
        conn_succ[bytes_rec] = '\0';
        printf("Message from server: %s\n", conn_succ);
    }

    // now we need to send the client connected mode to the server, [PUBLISHER | SUBSCRIBER]
    
    int mode_ss = send(client_fd, MODE, strlen(MODE), 0);
    if (mode_ss < 0) {
        close(client_fd);
        printf("Couldn't send the mode to the server!\n");
        exit(EXIT_FAILURE);
    }

    // now i need to create two theads to handle send and receive loops
    // below thing doesn't work anymore
//    char client_command[4096];
//    while (1) {
//        fgets(client_command, sizeof(client_command), stdin);
//        int comm_s = send(client_fd, (char *)&client_command, sizeof(client_command), 0);
//        if (strcmp(client_command, "exit\n" ) == 0) {
//            close(client_fd);
//            break;
//        } 
//        if (comm_s < 0) {
//            perror("Couldn't send the message to the server! \n");
//            exit(EXIT_FAILURE);
//        }
//    }

    // creating two threads to send messages to the server and receive messages from publishers
    pthread_t t_send, t_recv;
    if (pthread_create(&t_send, NULL, send_thread_func, NULL) != 0) {
        perror("Failed to create send thread!\n");
        close(client_fd);
    }
    if (pthread_create(&t_recv, NULL, recv_thread_func, NULL) != 0) {
        perror("Failed to create receive thread!\n");
        close(client_fd);
    }
    // couple those threads with the main so none will terminate while main is active  
    // we need to keep these two active while main is alive
    pthread_join(t_recv, NULL);
    pthread_join(t_send, NULL);


    return 0;

}
