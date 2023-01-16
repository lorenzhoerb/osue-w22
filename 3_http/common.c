/**
 * @file common.c
 * @author Lorenz Hörburger 12024737
 * @brief Common mothods for http client and http server
 *
 * @version 0.1
 * @date 15.01.2023
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "common.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void get_rfc822_date(char* date)
{
    char outstr[200];
    time_t t;
    struct tm* tmp;
    t = time(NULL);
    tmp = localtime(&t);
    if (tmp == NULL) {
        log_error("localtime");
    }
    if (strftime(outstr, sizeof(outstr), "%a, %d %b %y %T %z", tmp) == 0) {
        log_error("strftime returned 0");
    }

    strcpy(date, outstr);
}

char* resolve_path(char* docRoot, char* rel_url, char* index)
{
    char* resolved = malloc(strlen(docRoot) + strlen(rel_url) + strlen(index) + 1);
    strcpy(resolved, docRoot);
    strcat(resolved, rel_url);
    char* file = file_from_url(rel_url);
    if (file == NULL) {
        strcat(resolved, index);
    }
    return resolved;
}

int file_size(FILE* file)
{
    int size;
    fseek(file, 0L, SEEK_END);
    size = ftell(file);
    fseek(file, 0L, SEEK_SET);
    return size;
}

int is_port_valid(const char* port)
{
    int parsed_port;
    int scanned = sscanf(port, "%d", &parsed_port);
    char* endptr;
    strtol(port, &endptr, 10);
    if (*endptr != '\0') {
        return 0;
    }

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

char* status_str(int status)
{
    switch (status) {
    case 200:
        return "OK";
    case 400:
        return "Bad Request";
    case 404:
        return "Not Found";
    case 501:
        return "Not implemented";
    default:
        return NULL;
    }
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
