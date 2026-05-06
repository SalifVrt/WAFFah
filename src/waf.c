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
  // signatures to detect
  const char* xss = "<script>";
  const char* sqli = "OR 1=1";
  const char* lfi = "../";

  if (strstr(request, xss)) {
    printf("XSS attack detected.\n");
    return 0;
  }
  if (strstr(request, sqli)) {
    printf("SQL injection attack detected.\n");
    return 0;
  }
  if (strstr(request, lfi)) {
    printf("Path Traversal (LFI) attack detected.\n");
    return 0;
  }

  return 1;
}
