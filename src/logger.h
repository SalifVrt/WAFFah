#ifndef LOGGER_H
#define LOGGER_H

void log_transaction(const char* client_ip, const char* request_line,
                     int blocked);
// save request to log file, blocked -> 1 if blocked by WAF, else 0

#endif