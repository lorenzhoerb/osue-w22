/**
 * @file intmul.c
 * @author Lorenz Hörburger 12024737
 * @brief Takes hex strings from stdin and multiplies them.
 *
 * @version 0.1
 * @date 2022-09-12
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "hexlib.h"
#include "mulutil.h"
#include <stdio.h>
#include <string.h>

char* a = NULL;
char* b = NULL;

void parse_input(int argc, char** argv);
void mult_hex(char** result, char* hex_a, char* hex_b);

/**
 * @brief Takes hex_a hex_a with equal length and lenth must be a power of two
 *
 * @param result
 * @param hex_a
 * @param hex_b
 */
void mult_hex(char** result, char* hex_a, char* hex_b)
{
    size_t hex_len = strlen(hex_a);

    if (hex_len == 1) {
        int result = mult_hex_chars(hex_a[0], hex_b[0]);
        printf("%x\n", result);
        return;
    }

    struct SubHex* subhex = init_sub_hex(hex_a, hex_b);
    if (subhex == NULL) {
        log_error("failed init subex\n");
    }

    // pipes[part][direction: 0 write arguments 1 get solution][pipe]
    int pipes[4][2][2];
    int pids[4];

    // forks and runs child processes
    setup_children(pipes, pids);

    // writes subhex to child processes
    // for sub calculation
    write_subhex(pipes, subhex);

    // read results from children
    // [0]: ah*bh, [1]: ah*bl, [2]: al*bh, [3]: al*bl
    char sub_results[4][hex_len * 2 + 1];
    read_results(hex_len * 2 + 1, sub_results, pipes, pids);

    char* r1 = add_hex_str(sub_results[3], sub_results[0], hex_len);
    char* r2 = add_hex_str(r1, sub_results[2], hex_len / 2);
    char* r3 = add_hex_str(r2, sub_results[1], hex_len / 2);

    // sets leading zeros to power of 2
    // remove_zeros(r3);
    set_leading_zeros(&r3);

    printf("%s\n", r3);
    free(r1);
    free(r2);
    free(r3);
    close_sub_hex(subhex);
}

/**
 * @brief Checks if a string is a valid hex string and allocates memory for it.
 *
 * @param line line
 * @param read length of read line
 * @return char* allocated hex string
 */
char* parse_hex_line(char* line, ssize_t read)
{
    if (line[read - 1] == '\n') {
        line[read - 1] = '\0';
    }

    if (!is_hex_str(line)) {
        log_error("Invlaid hex string: %s", line);
        free(line);
        exit(EXIT_FAILURE);
    }

    char* hex = (char*)malloc(sizeof(char) * strlen(line) + 1);
    if (hex == NULL) {
        log_error("Error while malloc");
        free(line);
        exit(EXIT_FAILURE);
    }
    strcpy(hex, line);
    return hex;
}

/**
 * @brief Parses first two line of stdin to hexvalus and sets them to the global
 * variables a and b.
 *
 * @param argc arg count
 * @param argv  argv verctor
 */
void parse_input(int argc, char** argv)
{
    char* line = NULL;
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
        log_error("Hex strigns could not be equalized\n");
        free(tmp_a);
        free(tmp_b);
        free(line);
        exit(EXIT_FAILURE);
    }
    a = tmp_a;
    b = tmp_b;
    free(line);
}

/**
 * @brief Main starting point of the program
 *
 * @param argc arg count
 * @param argv arg vector
 * @return int exit status
 */
int main(int argc, char** argv)
{
    prg_name = argv[0];
    char* result = NULL;

    parse_input(argc, argv);
    mult_hex(&result, a, b);

    free(a);
    free(b);
    free(result);
    exit(EXIT_SUCCESS);
    return 0;
}
