#include "proxy.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static ConnectionState connections[MAX_CLIENTS];

void start_proxy(int local_port, const char* remote_ip, int remote_port) {
  /*(void)local_port; (void)remote_ip; (void)remote_port; //only for warnings,
  will delete later

  printf("proxy will be coming soon !\n");*/

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

  // temp loop for testing
  while (1) {
    sleep(1);
  }
}
