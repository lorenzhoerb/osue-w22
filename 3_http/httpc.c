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
        return NULL;
    }

    int sockfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
    if (sockfd < 0) {
        log_error("Httpc socket failed");
        return NULL;
    }

    if (connect(sockfd, ai->ai_addr, ai->ai_addrlen) == -1) {
        log_error("Httpc connect failed");
        return NULL;
    }

    FILE* sockfile = fdopen(sockfd, "r+");
    if (sockfile == NULL) {
        log_error("Httpc open sockfile failed");
        return NULL;
    }

    return sockfile;
}

static int parse_res_line(char* line)
{

    char* token = strtok(line, " ");
    int i = 0;
    long int status = -1;
    while (token != NULL) {
        if (i == 0) {
            // protorol
            if (strcmp(token, PROTOCOL) != 0) {
                return -1;
            }
        } else if (i == 1) {
            // Status
            char* endptr;
            status = strtol(token, &endptr, 10);
            if (*endptr != '\0') {
                return -1;
            }
        }
        i++;
        token = strtok(NULL, " ");
    }

    if (i < 3) {
        return -1;
    }

    return status;
}

int httpc(const char* method, const char* url, const char* port, FILE* output)
{
    FILE* sockfile = create_socket(url, port);
    if (sockfile == NULL) {
        return -1;
    }

    send_request(sockfile, method, url);

    char* line = NULL;
    size_t size = 0;
    ssize_t read;

    int body = 0;
    int firstline = 1;
    while ((read = getline(&line, &size, sockfile)) != -1) {
        if (firstline) {
            char* dup = strdup(line);
            int status = parse_res_line(dup);
            if (status == -1) {
                fprintf(stderr, "Protocol Error!\n");
                free(dup);
                free(line);
                fclose(sockfile);
                exit(2);
            } else if (status != 200) {
                char* status_msg = strchr(line, ' ') + 1;
                fprintf(stderr, "%s", status_msg);
                free(dup);
                exit(3);
                break;
            }
            firstline = 0;
            free(dup);
        }
        if (strcmp(line, "\r\n") == 0) {
            body = 1;
            continue;
        }
        if (body) {
            fprintf(output, "%s", line);
        }
    }
    free(line);
    return 1;
}
