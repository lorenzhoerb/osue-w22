#include "hexlib.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <unistd.h>

const char* prg_name;

char* a = NULL;
char* b = NULL;

void log_error(const char* format, ...);
void parse_input(int argc, char** argv);
void mult_hex(char** result, char* hex_a, char* hex_b);

void write_arguments(int pipes[4][2][2], int pipe_index, const char* arg1,
    const char* arg2)
{
    write(pipes[pipe_index][0][1], arg1, strlen(arg1));
    write(pipes[pipe_index][0][1], "\n", 1);
    write(pipes[pipe_index][0][1], arg2, strlen(arg2));
    write(pipes[pipe_index][0][1], "\n", 1);
    if (close(pipes[pipe_index][0][1]) < 0) {
        fprintf(stderr, "error\n");
    }
}

void init_pipes(int pipes[4][2][2])
{
    int i;
    for (i = 0; i < 4; i++) {
        int writer = pipe(pipes[i][0]);
        int reader = pipe(pipes[i][1]);
        if (writer == -1 || reader == -1) {
            // handle error
            log_error("opening pipe failed");
        }
    }
}

void setup_pipes(pid_t pid, int parent_index, int pipes[4][2][2])
{
    // pipes[part][direction: 0 write arguments 1 get solution][pipe]
    if (pid == 0) {
        if (close(pipes[parent_index][0][1]) < 0) {
            fprintf(stderr, "error\n");
        }
        if (close(pipes[parent_index][1][0]) < 0) {
            fprintf(stderr, "error\n");
        }
        if (dup2(pipes[parent_index][0][0], STDIN_FILENO) < 0) {
            fprintf(stderr, "error\n");
        }
        if (dup2(pipes[parent_index][1][1], STDOUT_FILENO) < 0) {
            fprintf(stderr, "error\n");
        }
    } else {
        close(pipes[parent_index][0][0]);
        close(pipes[parent_index][1][1]);
    }
}

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
        printf("%X\n", result);
        return;
    }

    size_t part_len = (hex_len / 2);
    size_t part_buf_len = part_len + 1; // +1 for null-terminator

    char ah[part_buf_len];
    char al[part_buf_len];
    char bh[part_buf_len];
    char bl[part_buf_len];
    strncpy(ah, hex_a, part_len);
    strncpy(al, hex_a + part_len, part_len);
    strncpy(bh, hex_b, part_len);
    strncpy(bl, hex_b + part_len, part_len);
    ah[part_len] = '\0';
    al[part_len] = '\0';
    bh[part_len] = '\0';
    bl[part_len] = '\0';

    int pipes[4][2][2]; //[part][0 read / 1 write process][pipe]
    int pids[4];
    init_pipes(pipes);

    int i;
    for (i = 0; i < 4; i++) {
        pid_t pid = fork();
        setup_pipes(pid, i, pipes);
        if (pid == 0) {
            if (execlp(prg_name, prg_name, NULL) < 0) {
                log_error("execlp failed");
            }
            log_error("execlp failed. this should never be reached");
        }
        if (pid == -1) {
            log_error("pid error");
            // ToDo Error exit
        }
        pids[i] = pid;
    }

    write_arguments(pipes, 0, ah, bh);
    write_arguments(pipes, 1, ah, bh);
    write_arguments(pipes, 2, al, bh);
    write_arguments(pipes, 3, al, bl);
    wait(NULL);
    wait(NULL);
    wait(NULL);
    wait(NULL);
    char b1[20];
    char b2[20];
    char b3[20];
    char b4[20];
    ssize_t s = 0;
    s = read(pipes[0][1][0], b1, 20);
    b1[s - 1] = '\0';
    close(pipes[0][1][0]);
    s = read(pipes[1][1][0], b2, 20);
    b2[s - 1] = '\0';
    s = read(pipes[2][1][0], b3, 20);
    b3[s - 1] = '\0';
    s = read(pipes[3][1][0], b4, 20);
    b4[s - 1] = '\0';
    printf("%s\n", b1);
    printf("%s\n", b2);
    printf("%s\n", b3);
    printf("%s\n", b4);
}

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
    }
    strcpy(hex, line);
    return hex;
}

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
