#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

long PORT = 0;

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
    server_addr.sin_port = htons(4221); // convert from little endian to big endian since TCP/IP expect big-end
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

    printf("Waiting for a client to connect.....\n");



    return 0;

}
