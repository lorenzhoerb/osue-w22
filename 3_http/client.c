/**
 * @file client.c
 * @author Lorenz Hörburger 12024737
 * @brief Http client
 *
 * @version 0.1
 * @date 15.01.2023
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "common.h"
#include "httpc.h"
#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STD_FILE "index.html"

struct options {
    FILE* out;
    char* port;
    char* url;
};

static struct options* g_opts;

static void clean_exit(int exit_status);
static FILE* open_out_file(const char* file);

/**
 * @brief Prints the usage of the program.
 * 
 */
void usage(void)
{
    (void)fprintf(stderr, "Usage: %s [-p PORT] [ -o FILE | -d DIR ] URL\n",
        prg_name);
}

/**
 * @brief Initializes the options of the program.
 * 
 * @param argc argument counter
 * @param argv argument vector
 * @return struct options initialized options struct
 */
static struct options init_options(int argc, char** argv)
{
    struct options opts;
    char opt;
    char* outFile;
    char* dir;
    int opt_p = 0;
    int opt_o = 0;
    int opt_d = 0;
    opts.port = "80";
    while ((opt = getopt(argc, argv, "p:o:d:")) != -1) {
        switch (opt) {
        case 'p':
            opt_p += 1;
            opts.port = optarg;
            break;
        case 'o':
            opt_o += 1;
            outFile = optarg;
            break;
        case 'd':
            opt_d += 1;
            dir = optarg;
            break;
        default:
            usage();
            clean_exit(EXIT_FAILURE);
            break;
        }
    }

    // too many options
    if (opt_p > 1 || opt_o > 1 || opt_d > 1) {
        log_error("Too many options");
        clean_exit(EXIT_FAILURE);
    }

    // either option d or o
    if (opt_d && opt_o) {
        log_error("Option '-d' and '-o' are set. Either '-d' or '-o' can be set.");
        clean_exit(EXIT_FAILURE);
    }

    // validate port
    if (opt_p) {
        if (!is_port_valid(opts.port)) {
            log_error("Invalid port. Port must be in ranche of 0 - 65535");
            clean_exit(EXIT_FAILURE);
        }
    }

    // check URL argument
    if (argc - 1 == optind) {
        opts.url = argv[optind];
    } else {
        usage();
        clean_exit(EXIT_FAILURE);
    }

    // validate URL
    if (!is_url_valid(opts.url)) {
        log_error("Invalid url: %s.", opts.url);
        clean_exit(EXIT_FAILURE);
    }

    if (opt_o) {
        opts.out = open_out_file(outFile);
    } else if (opt_d) {
        char* file = file_from_url(opts.url);
        if (file == NULL) {
            file = STD_FILE;
        }
        char strDir[strlen(file) + strlen(dir) + 2];
        sprintf(strDir, "%s/%s", dir, file);
        opts.out = open_out_file(strDir);
    } else {
        opts.out = stdout;
    }

    return opts;
}

/**
 * @brief Opens the file @code{file} in writemode.
 * Errors exits when output file couldn't be opened.
 * 
 * @param file file to open
 * @return FILE* opened file
 */
static FILE* open_out_file(const char* file)
{
    FILE* out = fopen(file, "w");
    if (out == NULL) {
        log_error("Error creating output file %s", file);
        clean_exit(EXIT_FAILURE);
    }
    return out;
}

/**
 * @brief Exits the program with the given exit code and
 * cleans up all claimed ressoureces.
 * 
 * @param exit_status exit status
 */
static void clean_exit(int exit_status)
{
    if (g_opts != NULL) {
        if (g_opts->out != NULL) {
            fclose(g_opts->out);
        }
    }
    exit(exit_status);
}

/**
 * @brief Starting point of the program the http client program.
 * 
 * @param argc argument counter
 * @param argv argument vector
 * @return int exit code
 */
int main(int argc, char** argv)
{
    prg_name = argv[0];
    struct options opts = init_options(argc, argv);
    g_opts = &opts;
    if (httpc("GET", opts.url, opts.port, opts.out) < 0) {
        log_error("HTTPC failed");
        clean_exit(EXIT_FAILURE);
    }

    if(opts.out != NULL) {
        fclose(opts.out);
    }

    exit(EXIT_SUCCESS);
    return 0;
}
