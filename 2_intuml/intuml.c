#include "hexlib.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *prg_name;

char *a = NULL;
char *b = NULL;

void log_error(const char *format, ...);
void parse_input(int argc, char **argv);

char *parse_hex_line(char *line, ssize_t read) {
    if (line[read - 1] == '\n') {
        line[read - 1] = '\0';
    }

    if (!is_hex_str(line)) {
        log_error("Invlaid hex string: %s", line);
        free(line);
        exit(EXIT_FAILURE);
    }

    char *hex = (char *)malloc(sizeof(char) * strlen(line) + 1);
    if (hex == NULL) {
        log_error("Error while malloc");
    }
    strcpy(hex, line);
    return hex;
}

void parse_input(int argc, char **argv) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    char *tmp_a = NULL, *tmp_b = NULL;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (tmp_a == NULL) {
            tmp_a = parse_hex_line(line, read);
            continue;
        }
        if (tmp_b == NULL) {
            tmp_b = parse_hex_line(line, read);
            break;
        }
    }
    if (equalize_zeros(&tmp_a, &tmp_b) == -1) {
        // ToDo handle Error
    }
    a = tmp_a;
    b = tmp_b;
}

/**
 * @brief Logs the error message specified in format to stderr.
 * prg_name needs to be set before calling this methode
 *
 * @param format Error message as formatted char array
 * @param ... format parameters
 */
void log_error(const char *format, ...) {
    va_list args;
    fprintf(stderr, "%s ERROR: ", prg_name);
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "\n");
}

int main(int argc, char **argv) {
    prg_name = argv[0];
    parse_input(argc, argv);
    printf("%s, %s\n", a, b);

    free(a);
    free(b);
    return 0;
}
