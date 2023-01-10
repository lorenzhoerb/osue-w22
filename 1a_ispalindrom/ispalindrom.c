/**
 * @file is_palindrom.c
 * @date 06.11.2022
 *
 * @brief Main program module.
 * This program validates if a input is a palindrom.
 * Inputs can be stdin or 0..* FILES.
 * White spaces and case can be ignored. Writes output to stdout or a file ([-o
 *FILE]) USAGE: %s [-s] [-i] [-o outfile] [file...]
 **/
#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char* prg_name;

typedef struct option {
    int opt_s;
    int opt_i;
    FILE* out;
} option_t;

// Prototypes
void handle_options(int argc, char** argv, option_t* option);
void write_palindrom(FILE* in, FILE* out, int ignoreSpaces, int ignoreCase);
int is_palindrom(char* word, int ignoreSpaces, int ignoreCase);
void log_error(const char* format, ...);
void usage(void);

/**
 * Program entry point.
 * @brief The program starts here. Handles the options and
 * handles the program ispalindrom.
 *
 * @param argc The argument counter.
 * @param argv The argument vector.
 * @return Returns EXIT_SUCCESS.
 */
int main(int argc, char** argv)
{
    prg_name = argv[0];
    option_t opts = { .opt_s = 0, .opt_i = 0, stdout };
    handle_options(argc, argv, &opts);

    int inputFileCount = argc - optind;
    int fileIndex;
    FILE* in = stdin;

    // read from stdin
    if (inputFileCount == 0) {
        write_palindrom(in, opts.out, opts.opt_s, opts.opt_i);
        fclose(opts.out);
        exit(EXIT_SUCCESS);
    }

    // read from files
    for (fileIndex = 0; fileIndex < inputFileCount; fileIndex++) {
        in = fopen(argv[optind + fileIndex], "r");
        if (in == NULL) {
            log_error("Could not open file: %s", strerror(errno));
            fclose(opts.out);
            exit(EXIT_FAILURE);
        }
        write_palindrom(in, opts.out, opts.opt_s, opts.opt_i);
        fclose(in);
    }

    fclose(opts.out);
    exit(EXIT_SUCCESS);
    return 0;
}

/**
 * Hanles user input and sets options
 * @brief This function handles the user input and checks the options s, i ,o
 * i ... ignore case
 * s ... ignore whitespaces
 * o ... outputfile
 *
 * @param argc argument count from main
 * @param argv argument vector from main
 * @param opt  options
 */
void handle_options(int argc, char** argv, option_t* option)
{
    char opt;
    int opt_o = 0;
    while ((opt = getopt(argc, argv, "sio:")) != -1) {
        switch (opt) {
        case 's':
            option->opt_s = 1;
            break;
        case 'i':
            option->opt_i = 1;
            break;
        case 'o':
            opt_o++;
            if (opt_o > 1) {
                log_error("Too many options: option o can only be set once");
                fclose(option->out);
                usage();
                exit(EXIT_FAILURE);
            }

            if ((option->out = fopen(optarg, "w+")) == NULL) {
                log_error("Creating output file failed: %s", strerror(errno));
                exit(EXIT_FAILURE);
            }
            break;
        default:
            usage();
            exit(EXIT_FAILURE);
        }
    }
}

/**
 * @brief Checks if the given parameter word is a palindrom.
 * The flags ignoreSpaces and ignoreCase are further options to
 * check for a palindrom.
 *
 * @param word word to be checked as a palindrom
 * @param ignoreSpaces ignores spaces when checking for a palindrom
 * @param ignoreCase ignores the case when checking for a palindrom
 * @return int returns 1 if the word is a palindrom else 0
 */
int is_palindrom(char* word, int ignoreSpaces, int ignoreCase)
{
    char* start = word;
    char* end = word + strlen(word) - 1;
    char cpyL;
    char cpyR;

    while (start <= end) {
        if (ignoreSpaces) {
            while (*start == ' ') {
                start++;
            }
            while (*end == ' ') {
                end--;
            }
        }
        cpyL = *start;
        cpyR = *end;
        if (ignoreCase) {
            cpyL = toupper(cpyL);
            cpyR = toupper(cpyR);
        }
        if (cpyL != cpyR) {
            return 0;
        }
        start++;
        end--;
    }
    return 1;
}

/**
 * @brief Reads the lines from in and checks if
 * its a palindrom and writes the outpute to out.
 * If the read line is a palindrom "line is a palindrom" gets
 * written to the out else "line is not a palindrom"
 *
 * @param in opened file to read lines
 * @param out opened file to write ispalindrom
 * @param ignoreSpaces ignores spaces when checking for a palindrom
 * @param ignoreCase ignores case whenchecking for a palindrom
 */
void write_palindrom(FILE* in, FILE* out, int ignoreSpaces, int ignoreCase)
{
    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, in)) != -1) {
        if (line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }

        if (is_palindrom(line, ignoreSpaces, ignoreCase)) {
            fprintf(out, "%s is a palindrom\n", line);
        } else {
            fprintf(out, "%s is not a palindrom\n", line);
        }
    }
    free(line);
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

/**
 * @brief Prints the usage to stdout
 *
 */
void usage(void)
{
    (void)fprintf(stderr, "Usage: %s [-s] [-i] [-o outputfile] [file...]\n",
        prg_name);
}
