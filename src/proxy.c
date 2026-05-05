#include <stdio.h>
#include "proxy.h"

static ConnectionState connections[MAX_CLIENTS];

void start_proxy(int local_port, const char* remote_ip, int remote_port){
    (void)local_port; (void)remote_ip; (void)remote_port; //only for warnings, will delete later

    printf("proxy will be coming soon !\n");
}
