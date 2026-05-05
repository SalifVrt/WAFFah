#ifndef PROXY_H
#define PROXY_H

#define MAX_CLIENTS 10
#define BUFFER_SIZE 4096

//client-server link connection struct
typedef struct {
    int client_fd;
    int server_fd;       //socket to localhost:8080
    int is_active;       //1 if active, else 0
} ConnectionState;

void start_proxy(int local_port, const char* remote_ip, int remote_port);

#endif