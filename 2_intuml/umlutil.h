#ifndef UMLUTIL
#define UMLUTIL

#include <stdlib.h>

struct SubHex {
    size_t size;
    char* ah;
    char* al;
    char* bh;
    char* bl;
};

const char* prg_name;

/**
 * @brief inits s aubhex stuct. Hex_a and Hex_b b must be equal length
 * and must be devidable by 2.
 *
 * @param hex_a hex string a
 * @param hex_b hex string b
 * @return struct SubHex*
 */
struct SubHex* init_sub_hex(char* hex_a, char* hex_b);

/**
 * @brief Closes all opened memory allocation of sub hex
 * - including subhex.
 *
 * @param subhex
 */
void close_sub_hex(struct SubHex* subhex);

/**
 * @brief writes the two hex arguments for subcalculation to the given pipe
 *
 */
void write_arguments(int pipes[4][2][2], int pipe_index, const char* arg1,
    const char* arg2);

void write_subhex(int pipes[4][2][2], struct SubHex* subhex);

void setup_pipes(pid_t pid, int parent_index, int pipes[4][2][2]);

void init_pipes(int pipes[4][2][2]);

void setup_children(int pipes[4][2][2], int pids[4]);

void read_results(int res_size, char result[4][res_size], int pipes[4][2][2]);

/**
 * @brief Logs the error message specified in format to stderr.
 * prg_name needs to be set before calling this methode
 *
 * @param format Error message as formatted char array
 * @param ... format parameters
 */
void log_error(const char* format, ...);

#endif
