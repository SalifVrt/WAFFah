#ifndef WAF_H
#define WAF_H

int inspect_request(const char* request);
//analyses entering buffer, returns 1 if clean request, else 0 (attack detected)

#endif