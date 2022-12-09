/**
 * @file multutil.c
 * @author Lorenz Hörburger 12024737
 * @brief Util functionalities for intmul
 *
 * @version 0.1
 * @date 2022-09-12
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "mulutil.h"
#include "stdio.h"
#include <errno.h>
#include <signal.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/**
 * @brief Inits all sub hex values: ah, al, bh, bl
 * and safes it into the struct SubHex. Memory gets allocated
 * and subHex should be freed with close_sub_hex;
 *
 * @param hex_a hex string a
 * @param hex_b hex string b
 * @return struct SubHex*
 */
struct SubHex* init_sub_hex(char* hex_a, char* hex_b)
{
    struct SubHex* subhex = malloc(sizeof(struct SubHex));
    if (subhex == NULL) {
        return NULL;
    }
    size_t hex_len = strlen(hex_a);
    size_t part_len = (hex_len / 2);
    subhex->ah = malloc(part_len + 1);
    subhex->al = malloc(part_len + 1);
    subhex->bh = malloc(part_len + 1);
    subhex->bl = malloc(part_len + 1);
    subhex->size = part_len;

    if (subhex->ah == NULL || subhex->al == NULL || subhex->bh == NULL || subhex->bl == NULL) {
        return NULL;
    }
    strncpy(subhex->ah, hex_a, part_len);
    strncpy(subhex->al, hex_a + part_len, part_len);
    strncpy(subhex->bh, hex_b, part_len);
    strncpy(subhex->bl, hex_b + part_len, part_len);

    subhex->ah[part_len] = '\0';
    subhex->al[part_len] = '\0';
    subhex->bh[part_len] = '\0';
    subhex->bl[part_len] = '\0';

    return subhex;
}

/**
 * @brief Frees all allocated memory in subhex
 *
 * @param subhex
 */
void close_sub_hex(struct SubHex* subhex)
{
    free(subhex->ah);
    free(subhex->al);
    free(subhex->bh);
    free(subhex->bl);
    free(subhex);
}

/**
 * @brief Writes an argument to the child via pipes.
 *
 * @param pipes pipes
 * @param pipe_index pipe index
 * @param arg1 hex string
 * @param arg2 hex string
 */
void write_arguments(int pipes[4][2][2], int pipe_index, const char* arg1,
    const char* arg2)
{
    write(pipes[pipe_index][0][1], arg1, strlen(arg1));
    write(pipes[pipe_index][0][1], "\n", 1);
    write(pipes[pipe_index][0][1], arg2, strlen(arg2));
    write(pipes[pipe_index][0][1], "\n", 1);
    if (close(pipes[pipe_index][0][1]) < 0) {
        fprintf(stderr, "error\n");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Writes the SubHex to child pipes.
 *
 * @param pipes pipes
 * @param subhex SubHex struct
 */
void write_subhex(int pipes[4][2][2], struct SubHex* subhex)
{
    write_arguments(pipes, 0, subhex->ah, subhex->bh);
    write_arguments(pipes, 1, subhex->ah, subhex->bl);
    write_arguments(pipes, 2, subhex->al, subhex->bh);
    write_arguments(pipes, 3, subhex->al, subhex->bl);
}

/**
 * @brief Waits for the child proccesses to terminate and reads the results into result.
 * [0]: ah*bh, [1]: ah*bl, [2]: al*bh, [3]: al*bl
 *
 * @param res_size Size of the buffter where the result should be saved
 * @param result Result
 * @param pipes pupes
 * @param pids Process ids of child processes
 */
void read_results(int res_size, char result[4][res_size], int pipes[4][2][2], int pids[4])
{
    int status;
    int i;
    for (i = 0; i < 4; i++) {
        waitpid(pids[i], &status, 0);
        if (WEXITSTATUS(status) == EXIT_FAILURE) {
            fprintf(stderr, "error exit because child");
            exit(EXIT_FAILURE);
        }
    }

    ssize_t s = 0;
    for (i = 0; i < 4; i++) {
        s = read(pipes[i][1][0], result[i], res_size);
        result[i][s - 1] = '\0';
    }
}

/**
 * @brief Set the up pipes and closes all unneeded pipes.
 *
 * @param pid process id
 * @param parent_index index of parent. Also identifier for pipe
 * @param pipes inited pipes
 */
void setup_pipes(pid_t pid, int parent_index, int pipes[4][2][2])
{
    // pipes[part][direction: 0 write arguments 1 get solution][pipe]
    if (pid == 0) {
        if (close(pipes[parent_index][0][1]) < 0) {
            fprintf(stderr, "error\n");
            exit(EXIT_FAILURE);
        }
        if (close(pipes[parent_index][1][0]) < 0) {
            fprintf(stderr, "error\n");
            exit(EXIT_FAILURE);
        }
        if (dup2(pipes[parent_index][0][0], STDIN_FILENO) < 0) {
            fprintf(stderr, "error\n");
            exit(EXIT_FAILURE);
        }
        if (dup2(pipes[parent_index][1][1], STDOUT_FILENO) < 0) {
            fprintf(stderr, "error\n");
            exit(EXIT_FAILURE);
        }
    } else {
        close(pipes[parent_index][0][0]);
        close(pipes[parent_index][1][1]);
    }
}

/**
 * @brief Inits all pipes.
 *
 * @param pipes pipes
 */
void init_pipes(int pipes[4][2][2])
{
    int i;
    for (i = 0; i < 4; i++) {
        int writer = pipe(pipes[i][0]);
        int reader = pipe(pipes[i][1]);
        if (writer == -1 || reader == -1) {
            // handle error
            log_error("opening pipe failed");
            exit(EXIT_FAILURE);
        }
    }
}

/**
 * @brief Set the up children. And inits pids.
 *
 * @param pipes inited pipes
 * @param pids pids
 */
void setup_children(int pipes[4][2][2], int pids[4])
{
    init_pipes(pipes);

    int i;
    for (i = 0; i < 4; i++) {
        pid_t pid = fork();
        setup_pipes(pid, i, pipes);
        if (pid == 0) {
            if (execlp(prg_name, prg_name, NULL) < 0) {
                log_error("execlp failed");
                exit(EXIT_FAILURE);
            }
            log_error("execlp failed. this should never be reached");
        }
        if (pid == -1) {
            int j = 0;
            for (j = 0; j < i; j++) {
                kill(pids[i], SIGKILL);
            }
            log_error("pid error: %s", strerror(errno));
            exit(EXIT_FAILURE);
        }
        pids[i] = pid;
    }
}

/**
 * @brief Logs an error to stderr with custom format.
 *
 * @param format printf format
 * @param ... params for printf
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
