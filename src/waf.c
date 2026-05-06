#include "waf.h"
#include <string.h>
#include <stdio.h>

int inspect_request(const char* request){
    //signatures to detect
    const char *xss = "<script>";
    const char *sqli = "OR 1=1";
    const char *lfi = "../";

    if (strstr(request, xss)){
        printf("XSS attack detected.\n");
        return 0;
    }
    if (strstr(request, sqli)){
        printf("SQL injection attack detected.\n");
        return 0;
    }
    if (strstr(request, lfi)){
        printf("Path Traversal (LFI) attack detected.\n");
        return 0;
    }

    return 1;
}
