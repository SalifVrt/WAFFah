#include "logger.h"

#include <stdio.h>
#include <time.h>

void log_transaction(const char* client_ip, const char* request_line,
                     int blocked) {
  time_t now = time(NULL);  // current time
  struct tm* t = localtime(&now);
  char time_str[64];
  strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", t);

  FILE* file = fopen("waf.log", "a");
  if (file == NULL) {
    return;
  }

  const char* status = blocked ? "BLOCKED" : "ALLOWED";
  fprintf(file, "[%s] [%s] [%s] %s\n", time_str, client_ip, status,
          request_line);
  fclose(file);
}