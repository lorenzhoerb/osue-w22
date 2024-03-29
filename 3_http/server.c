/**
 * @file client.c
 * @author Lorenz Hörburger 12024737
 * @brief CLI HTTP Server
 *
 * @version 0.1
 * @date 15.01.2023
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "common.h"
#include "https.h"
#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#define PROTOCOL "HTTP/1.1"

struct options {
    char* port;
    char* index;
    char* docRoot;
};

struct options* g_opts;

void clean_exit(int exit_status);
struct options init_options(int argc, char** argv);

/**
 * @brief Prints the usage of the program
 * 
 */
void usage(void)
{
    (void)fprintf(stderr, "Usage: %s [-p PORT] [-i INDEX] DOC_ROOT\n",
        prg_name);
}

/**
 * @brief Inits the options.
 * 
 * @param argc argc argument counter
 * @param argv argv argument vector
 * @return struct options initilaized option
 */
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

    if (opt_p && !is_port_valid(opts.port)) {
        log_error("Invalid port. Port must be in ranche of 0 - 65535");
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

/**
 * @brief Exits with the given exit status and cleans up all claimed ressources
 * 
 * @param exit_status exit status
 */
void clean_exit(int exit_status)
{
    exit(exit_status);
}

/**
 * @brief HTTP requst handler
 * 
 * @param req request strcut
 * @param res response struct
 */
void handler(struct req* req, struct res* res)
{
    if (strcmp(req->method, "GET") == 0) {
        char* reqfilepath = resolve_path(req->settings->docRoot, req->path, req->settings->index);
        if ((res->body = fopen(reqfilepath, "r")) == NULL) {
            res->status = 404;
        } else {
            res->status = 200;
        }
        free(reqfilepath);
    } else {
        res->status = 501;
    }
}

/**
 * @brief Handles SIGTERM and SIGINT. Shuts the http server down.
 * 
 * @param signal signal
 */
void handle_signal(int signal)
{
    if (signal == SIGTERM || signal == SIGINT) {
        errno = 0;
        server_shutdown();
    }
}

/**
 * @brief Main method of HTTP SERVER CLI.
 * 
 * @param argc argument counter
 * @param argv argument vector
 * @return int exit status
 */
int main(int argc, char** argv)
{
    prg_name = argv[0];

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_signal;

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    struct options opts = init_options(argc, argv);
    g_opts = &opts;

    struct settings settings;
    settings.docRoot = opts.docRoot;
    settings.index = opts.index;

    int sockfd = create_server(opts.port);
    server_listen(sockfd, 1, handler, &settings);

    exit(EXIT_SUCCESS);
    return 0;
}
