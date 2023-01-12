#include "common.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

struct options {
    char* port;
    char* index;
    char* docRoot;
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

void clean_exit(int exit_status)
{
    exit(exit_status);
}

int main(int argc, char** argv)
{
    prg_name = argv[0];
    printf("Hello from server\n");
    struct options opts = init_options(argc, argv);
    printf("%s, %s, %s\n", opts.port, opts.index, opts.docRoot);

    return 0;
}
