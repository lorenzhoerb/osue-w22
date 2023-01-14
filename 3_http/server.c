#include "common.h"
#include <getopt.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#define PROTOCOL "HTTP/1.1\r\n"

struct options {
    char* port;
    char* index;
    char* docRoot;
};

struct req {
    char* path;
    char* method;
};

struct options* g_opts;

void clean_exit(int exit_status);

void usage(void)
{
    (void)fprintf(stderr, "Usage: %s [-p PORT] [-i INDEX] DOC_ROOT\n",
        prg_name);
}

struct options init_options(int argc, char** argv)
{
    struct options opts;
    char opt;
    int opt_p = 0;
    int opt_i = 0;
    opts.port = "80";
    opts.index = "index.html";
    while ((opt = getopt(argc, argv, "p:i:")) != -1) {
        switch (opt) {
        case 'p':
            opt_p += 1;
            opts.port = optarg;
            break;
        case 'i':
            opt_i += 1;
            opts.index = optarg;
            break;
        default:
            usage();
            clean_exit(EXIT_FAILURE);
            break;
        }
    }

    // too many options
    if (opt_p > 1 || opt_i > 1) {
        log_error("Too many options");
        clean_exit(EXIT_FAILURE);
    }

    // check doc_root argument
    if (argc - 1 == optind) {
        opts.docRoot = argv[optind];
    } else {
        usage();
        clean_exit(EXIT_FAILURE);
    }

    return opts;
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
    }

    int sockfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);

    if (sockfd < 0) {
        log_error("Socket failed");
    }

    int optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

    if (bind(sockfd, ai->ai_addr, ai->ai_addrlen) < 0) {
        log_error("bind failed");
    }

    freeaddrinfo(ai);
    return sockfd;
}

void server_listen(int sockfd, int queue, void (*handle)(FILE*))
{
    if (listen(sockfd, queue) < 0) {
        log_error("listen failed");
    }

    while (1) {
        int clientfd = accept(sockfd, NULL, NULL);

        if (clientfd < 0) {
            log_error("accept failed");
        }

        FILE* clientfile = fdopen(clientfd, "r+");
        if (clientfile == NULL) {
            log_error("Httpc open sockfile failed");
        }

        (*handle)(clientfile);
        fclose(clientfile);
    }
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
            if (strcmp(token, PROTOCOL) != 0) {
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
    while ((read = getline(&line, &size, client)) != -1) {
        if (firstline) {
            dup = strdup(line);
            if (parse_req_line(dup, req) < 0) {
                return NULL;
            }
            firstline = 0;
        } else if (strcmp(line, "\r\n") != 0) {
            // header
            continue;
        } else {
            break;
        }
    }
    free(line);
    return req;
}

void handler(FILE* client)
{
    struct req* req = parse_req(client);
    if (req == NULL) {
        printf("PROTOCOL ERROR\n");
        return;
    }
    printf("Req: %s, %s\n", req->path, req->method);
}

void clean_exit(int exit_status)
{
    exit(exit_status);
}

int main(int argc, char** argv)
{
    prg_name = argv[0];
    struct options opts = init_options(argc, argv);
    printf("%s, %s, %s\n", opts.port, opts.index, opts.docRoot);
    int sockfd = create_server(opts.port);
    server_listen(sockfd, 1, handler);

    return 0;
}
