/**
 * @file https.c
 * @author Lorenz Hörburger 12024737
 * @brief HTTP Server API
 *
 * @version 0.1
 * @date 15.01.2023
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "https.h"
#include "common.h"
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#define PROTOCOL "HTTP/1.1"

volatile sig_atomic_t server_quit = 0;
char *gdup = NULL;


void server_shutdown(void)
{
    server_quit = 1;
}

int create_server(char* port)
{
    struct addrinfo hints, *ai;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int res = getaddrinfo(NULL, port, &hints, &ai);

    if (res != 0) {
        log_error("Getaddrinfo failed");
        exit(EXIT_FAILURE);
    }

    int sockfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);

    if (sockfd < 0) {
        log_error("Socket failed");
        exit(EXIT_FAILURE);
    }

    int optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

    if (bind(sockfd, ai->ai_addr, ai->ai_addrlen) < 0) {
        log_error("bind failed");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(ai);
    return sockfd;
}

void server_listen(int sockfd, int queue, void (*handle)(struct req*, struct res*), struct settings* settings)
{
    if (listen(sockfd, queue) < 0) {
        log_error("listen failed");
        exit(EXIT_FAILURE);
    }

    while (!server_quit) {
        int clientfd = accept(sockfd, NULL, NULL);
        if (clientfd < 0) {
            break;
        }

        FILE* clientfile = fdopen(clientfd, "r+");
        if (clientfile == NULL) {
            log_error("Httpc open sockfile failed");
        }

        struct req* req = parse_req(clientfile);
        struct res res = { .status = 400, .body = NULL };

        if (req != NULL) {
            // Server Log
            req->settings = settings;
            (*handle)(req, &res);
        }

        send_response(clientfile, &res);

        if (clientfile != NULL) {
            fclose(clientfile);
        }
        if (res.body != NULL) {
            fclose(res.body);
        }
    }
    free(gdup);
    close(sockfd);
}

void send_response(FILE* clientfile, struct res* res)
{
    char date[100];
    get_rfc822_date(date);

    //Response line
    fprintf(clientfile, "%s %d %s\r\n", PROTOCOL, res->status, status_str(res->status));

    if (res->status >= 200 && res->status < 300) {
        fprintf(clientfile, "Date: %s\r\n", date);
    }

    if (res->body != NULL) {
        int content_length = file_size(res->body);
        fprintf(clientfile, "Content-Length: %d\r\n", content_length);
    }

    fprintf(clientfile, "Connection: close\r\n");

    if (res->body != NULL) {
        // End header / Begin Body
        fprintf(clientfile, "\r\n");
        char* line = NULL;
        size_t size = 0;
        ssize_t read;
        while ((read = getline(&line, &size, res->body)) != -1) {
            fprintf(clientfile, "%s", line);
        }
    }
    fflush(clientfile);
}

int parse_req_line(char* line, struct req* req)
{
    char* token = strtok(line, " ");
    int i = 0;
    while (token != NULL) {
        if (i == 0) {
            // Method
            req->method = token;
        } else if (i == 1) {
            // Path
            req->path = token;
        } else if (i == 2) {
            // Protocol
            if (strcmp(token, "HTTP/1.1\r\n") != 0) {
                return -1;
            }
        }
        i++;
        token = strtok(NULL, " ");
    }

    if (i != 3) {
        return -1;
    }

    return 1;
}

struct req* parse_req(FILE* client)
{
    struct req* req = malloc(sizeof(struct req));
    req->path = NULL;
    req->method = NULL;
    char* line = NULL;
    char* dup;
    size_t size = 0;
    ssize_t read;
    int firstline = 1;
    while (1) {
        read = getline(&line, &size, client);
        if (read == -1 && errno != EINTR) {
            break;
        }
        if (read == -1 && errno == EINTR) {
            errno = 0;
            continue;
        }

        if (firstline) {
            dup = strdup(line);
            gdup = dup;
            firstline = 0;
            if (parse_req_line(dup, req) < 0) {
                free(req);
                req = NULL;
                continue;
            }
        } else if (strcmp(line, "\r\n") != 0) {
            // ToDo: parse header
            continue;
        } else {
            break;
        }
    }
    free(line);
    return req;
}
