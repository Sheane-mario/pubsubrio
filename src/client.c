#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int SERVER_PORT = 0;
char* SERVER_IP;

int main(int argc, char* argv[]) {

    // disable output buffering
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    if (argc < 3) {
        printf("Please provide the [SERVER_IP] and the [SERVER_PORT] !\n");
        return 1;
    }

    // port will be a string initially
    char* portstr;
    SERVER_PORT = strtol(argv[2], &portstr, 10);
    SERVER_IP = argv[1];

    return 0;

}
