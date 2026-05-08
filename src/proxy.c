#include "proxy.h"

#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>

#include "logger.h"
#include "waf.h"

static volatile int keep_running = 1;
static ConnectionState connections[MAX_CLIENTS];

void handle_sigint(int sig) {
  (void)sig;  // prevent unused parameter warning
  printf("\n[!] SIGINT received. shutting down waffah...\n");
  keep_running = 0;
}

int connect_to_server(const char* ip, int port) {
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) return -1;

  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  inet_pton(AF_INET, ip,
            &server_addr.sin_addr);  // converts IP to binary format

  if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
    close(sock);
    return -1;
  }
  return sock;  // returns real server's fd
}

void start_proxy(int local_port, const char* remote_ip, int remote_port) {
  int server_fd;
  struct sockaddr_in address;

  // socket opening
  server_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (server_fd < 0) {
    perror("error: socket creation");
    exit(EXIT_FAILURE);
  }

  // to avoid "address already in use" when relaunching program
  int opt = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  // preparing ip address and port
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(local_port);

  // binding
  int ret_bind = bind(server_fd, (struct sockaddr*)&address, sizeof(address));
  if (ret_bind < 0) {
    perror("error: socket binding");
    exit(EXIT_FAILURE);
  }

  // listening
  int ret_listen = listen(server_fd, MAX_CLIENTS);
  if (ret_listen < 0) {
    perror("error: socket listening");
    exit(EXIT_FAILURE);
  }

  printf("proxy listening on port %d...\n", local_port);

  for (int i = 0; i < MAX_CLIENTS; i++) {
    connections[i].is_active = 0;
  }

  fd_set readfds;  // sockets to watch

  signal(SIGINT, handle_sigint);  // to shutdown properly

  while (keep_running) {
    FD_ZERO(&readfds);
    FD_SET(server_fd, &readfds);

    int max_sd = server_fd;

    for (int i = 0; i < MAX_CLIENTS; i++) {
      if (connections[i].is_active) {
        FD_SET(connections[i].client_fd, &readfds);
        if (connections[i].client_fd > max_sd)
          max_sd = connections[i].client_fd;
        FD_SET(connections[i].server_fd, &readfds);
        if (connections[i].server_fd > max_sd)
          max_sd = connections[i].server_fd;
      }
    }

    int activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
    if (activity < 0) {
      if (errno == EINTR) {  // to avoid showing error on shutdown
                             // (interruption)
        continue;
      }
      perror("error: select");
      continue;
    }

    // new client wants to connect, we check server state
    if (FD_ISSET(server_fd, &readfds)) {
      struct sockaddr_in client_addr;  // to store client info
      socklen_t client_len = sizeof(client_addr);

      int new_socket =
          accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
      if (new_socket < 0) {
        perror("error: accept connection");
        continue;
      }
      printf("[+] new client connected.\n");

      int i;
      for (i = 0; i < MAX_CLIENTS; i++) {
        if (!connections[i].is_active) {
          connections[i].client_fd = new_socket;
          connections[i].is_active = 1;

          inet_ntop(AF_INET, &client_addr.sin_addr, connections[i].client_ip,
                    INET_ADDRSTRLEN);

          connections[i].server_fd = connect_to_server(remote_ip, remote_port);
          if (connections[i].server_fd < 0) {
            printf("[-] server unreachable, rejecting client.\n");
            close(new_socket);
            connections[i].is_active = 0;
          }
          break;
        }
      }
      if (i == MAX_CLIENTS) close(new_socket);  // no space left
    }

    // existing client is communicating
    for (int i = 0; i < MAX_CLIENTS; i++) {
      if (connections[i].is_active &&
          FD_ISSET(connections[i].client_fd,
                   &readfds)) {  // check client state
        char buffer[BUFFER_SIZE];

        // read client message
        int valread = read(connections[i].client_fd, buffer, BUFFER_SIZE - 1);

        if (valread == 0) {  // client left
          printf("[-] client %d disconnected.\n", i);
          close(connections[i].client_fd);
          close(connections[i].server_fd);
          connections[i].is_active = 0;
        } else if (valread > 0) {
          buffer[valread] = '\0';
          printf("\n--- REQUEST ---\n%s\n---------------\n", buffer);

          char first_line[256];
          sscanf(buffer, "%255[^\r\n]", first_line);

          url_decode(first_line);

          if (inspect_request(buffer) == 0) {  // attack detected
            printf("BLOCKED BY WAF. (slot %d)\n", i);

            log_transaction(connections[i].client_ip, first_line, 1);

            // http response
            const char* forbidden_response =
                "HTTP/1.1 403 Forbidden\r\n"
                "Content-Type: text/html\r\n"
                "Connection: close\r\n\r\n"
                "<h1>403 Forbidden - Blocked by WAF</h1>";
            send(connections[i].client_fd, forbidden_response,
                 strlen(forbidden_response), 0);

            close(connections[i].client_fd);
            close(connections[i].server_fd);
            connections[i].is_active = 0;

          } else {  // clean request
            log_transaction(connections[i].client_ip, first_line, 0);

            send(connections[i].server_fd, buffer, valread, 0);
          }
        }
      }

      if (connections[i].is_active &&
          FD_ISSET(connections[i].server_fd, &readfds)) {
        char buffer[BUFFER_SIZE];
        int valread = read(connections[i].server_fd, buffer, BUFFER_SIZE - 1);
        if (valread == 0) {
          printf("[-] server backend finished. (slot %d)\n", i);
          close(connections[i].client_fd);
          close(connections[i].server_fd);
          connections[i].is_active = 0;
        } else if (valread > 0) {
          send(connections[i].client_fd, buffer, valread, 0);
        }
      }
    }
  }

  // SHUTDOWN SEQUENCE
  printf("[*] closing main server socket.\n");
  close(server_fd);

  // close all active connections
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (connections[i].is_active) {
      printf("[*] closing connection for slot %d.\n", i);
      close(connections[i].client_fd);
      close(connections[i].server_fd);
    }
  }
  printf("[*] waffah shutdown complete.\n");
}
