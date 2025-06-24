#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

long PORT = 0;

int main(int argc, char* argv[]) {

    if (argc < 2) {
        printf("Please input the server port! \n");
        return 1;
    }

    // port will be a string initially
    char* portstr;
    PORT = strtol(argv[1], &portstr, 10);



    return 0;

}
