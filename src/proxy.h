#ifndef PROXY_H
#define PROXY_H

#include <arpa/inet.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 4096

/**
 * @struct ConnectionState
 * @brief Stores the state of a client-server pair connection.
 * * Tracks the file descriptors for both the client and the backend server, 
 * along with the client's IP and the active status of the slot.
 */
typedef struct {
  int client_fd;
  int server_fd;  // socket to localhost:8080
  int is_active;  // 1 if active, else 0
  char client_ip[INET_ADDRSTRLEN];
} ConnectionState;

/**
 * @brief Establishes a TCP connection to the specified backend server.
 * * @param ip The target backend server's IP address.
 * @param port The target backend server's port.
 * @return The socket file descriptor if successful, -1 otherwise.
 */
int connect_to_server(const char* ip, int port);

/**
 * @brief Starts the asynchronous proxy server event loop.
 * * Initializes the listener, handles signal registration, and uses select() 
 * to multiplex I/O. It manages dynamic memory for requests and routes 
 * data through the WAF engine.
 * * @param local_port The port the proxy listens on.
 * @param remote_ip The target backend IP address.
 * @param remote_port The target backend port.
 */
void start_proxy(int local_port, const char* remote_ip, int remote_port);

#endif