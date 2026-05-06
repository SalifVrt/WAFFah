#include "waf.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int hex_to_int(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  return 0;
}

void url_decode(char* str) {
  char *pstr = str, *pbuf = str;
  while (*pstr) {
    if (*pstr == '%' && pstr[1] && pstr[2]) {
      *pbuf++ = (hex_to_int(pstr[1]) << 4) | hex_to_int(pstr[2]);
      pstr += 3;
    } else if (*pstr == '+') {
      *pbuf++ = ' ';
      pstr++;
    } else {
      *pbuf++ = *pstr++;
    }
  }
  *pbuf = '\0';
}

int inspect_request(const char* request) {
  char decoded_request[4096];  // request copy to avoid changing the original
  strncpy(decoded_request, request, sizeof(decoded_request) - 1);
  decoded_request[sizeof(decoded_request) - 1] = '\0';

  url_decode(decoded_request);

  // blacklist definition
  const char* bad_payloads[] = {
      "<script>", "javascript:", "onerror=",      // XSS attacks
      "OR 1=1",   "DROP TABLE",  "UNION SELECT",  // SQL injections
      "../",      "/etc/passwd", "cmd.exe",       // LFI / path traversal
      NULL};

  const char* bad_urls[] = {"/admin", "/phpmyadmin",
                            "/.env",  // forbidden access
                            NULL};

  for (int i = 0; bad_payloads[i] != NULL; i++) {
    if (strstr(decoded_request, bad_payloads[i])) {
      printf("ATTACK DETECTED: %s\n", bad_payloads[i]);
      return 0;
    }
  }

  for (int i = 0; bad_urls[i] != NULL; i++) {
    if (strstr(decoded_request, bad_urls[i])) {
      printf("ATTACK DETECTED: %s\n", bad_urls[i]);
      return 0;
    }
  }

  return 1;
}
