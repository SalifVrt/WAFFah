#ifndef WAF_H
#define WAF_H

/**
 * @brief Decodes a URL-encoded string in-place.
 * * Replaces percent-encoded characters (e.g., "%20", "%3C") with their
 * corresponding ASCII characters. The modification is performed directly 
 * on the provided string buffer.
 * * @param str The null-terminated string to be decoded.
 */
void url_decode(char* str);

/**
 * @brief Inspects an HTTP request for malicious payloads.
 * * Creates a local copy of the request, decodes it, and scans for known 
 * attack signatures including Cross-Site Scripting (XSS), SQL Injection (SQLi), 
 * and Local File Inclusion (LFI).
 * * @param request The null-terminated HTTP request string to inspect.
 * @return 1 if the request is considered clean, 0 if a threat is detected.
 */
int inspect_request(const char* request);

#endif