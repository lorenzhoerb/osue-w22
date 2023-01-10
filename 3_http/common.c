#include "common.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

int is_port_valid(const char* port)
{
    int parsed_port;
    int scanned = sscanf(port, "%d", &parsed_port);
    if (scanned == 1 && parsed_port >= 0 && parsed_port <= 65535) {
        return 1;
    }
    return 0;
}

char* file_from_url(const char* url)
{
    char* lastSlash = strrchr(url, '/');
    char* file = lastSlash + 1;
    if ((lastSlash + 1)[0] == '\0') {
        return NULL;
    }
    return file;
}

char* file_path_from_url(const char* url)
{
    return strpbrk(url + 7, ";/?:@=&");
}

void host_from_url(const char* url, char* host)
{
    strcpy(host, url + 7);
    char* endHost = strpbrk(host, ";/?@=&");
    *endHost = '\0';
}

int is_url_valid(const char* url)
{
    const char* prefix = "http://";
    if (strstr(url, "http://") == NULL) {
        return 0;
    }

    char* lastSlash = strrchr(url, '/');
    if (lastSlash <= url + strlen(prefix)) {
        return 0;
    }
    return 1;
}

/**
 * @brief Logs the error message specified in format to stderr.
 * prg_name needs to be set before calling this methode
 *
 * @param format Error message as formatted char array
 * @param ... format parameters
 */
void log_error(const char* format, ...)
{
    va_list args;
    fprintf(stderr, "%s ERROR: ", prg_name);
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "\n");
}
