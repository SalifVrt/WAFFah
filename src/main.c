#include <stdio.h>
#include <stdlib.h>
#include "proxy.h"

int main(int argc, char *argv[]){
    int port = 8888; //default port

    if (argc > 1){
        port = atoi(argv[1]);
    }

    printf("starting WAFFah on port %d...\n", port);
    printf("redirecting traffic to 127.0.0.1:8080\n");

    start_proxy(port, "127.0.0.1", 8080);

    return 0;
}