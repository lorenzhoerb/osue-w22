#include "httpc.h"
#include "common.h"
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#define PROTOCOL "HTTP/1.1"

static void send_request(FILE* sockfile, const char* method, const char* url)
{
    char host[strlen(url) + 1];
    host_from_url(url, host);
    char* ressource = file_path_from_url(url);
    fprintf(sockfile, "%s %s %s\r\nHost: %s\r\nConnection: close\r\n\r\n", method, ressource, PROTOCOL, host);
}

static FILE* create_socket(const char* url, const char* port)
{
    char host[strlen(url) + 1];
    host_from_url(url, host);
    struct addrinfo hints, *ai;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int res = getaddrinfo(host, port, &hints, &ai);
    if (res != 0) {
        log_error("Httpc failed getaddrinfo failed");
    }

    int sockfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
    if (sockfd < 0) {
        log_error("Httpc socket failed");
    }

    if (connect(sockfd, ai->ai_addr, ai->ai_addrlen) == -1) {
        log_error("Httpc connect failed");
    }

    FILE* sockfile = fdopen(sockfd, "r+");
    if (sockfile == NULL) {
        log_error("Httpc open sockfile failed");
    }

    return sockfile;
}

int httpc(const char* method, const char* url, const char* port, FILE* output)
{
    FILE* sockfile = create_socket(url, port);
    send_request(sockfile, method, url);

    char* line = NULL;
    size_t size = 0;
    ssize_t read;

    int body = 0;
    while ((read = getline(&line, &size, sockfile)) != -1) {
        if (strcmp(line, "\r\n") == 0) {
            body = 1;
            continue;
        }
        if (body) {
            fprintf(output, "%s", line);
        }
    }

    if (line != NULL) {
        free(line);
    }
    return 1;
}
