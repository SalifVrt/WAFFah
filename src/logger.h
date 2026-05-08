#ifndef LOGGER_H
#define LOGGER_H

/**
 * @brief Logs a transaction to the WAF audit log file.
 * * Appends a formatted entry to "waf.log" containing a timestamp,
 * the client's IP address, the decision status (ALLOWED/BLOCKED),
 * and the first line of the HTTP request.
 * * @param client_ip The client's IP address as a string.
 * @param request_line The first line of the HTTP request.
 * @param blocked Integer flag: 1 if blocked by WAF, 0 if allowed.
 */
void log_transaction(const char* client_ip, const char* request_line,
                     int blocked);

#endif