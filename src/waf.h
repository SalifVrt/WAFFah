#ifndef WAF_H
#define WAF_H

void url_decode(char* str);  // decodes %XX to real char
int inspect_request(const char* request);
// analyses entering buffer, returns 1 if clean request, else 0 (attack
// detected)

#endif