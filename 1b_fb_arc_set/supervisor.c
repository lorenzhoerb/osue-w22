/**
 * @file supervisor.c
 * @author Lorenz Hörburger (120247373)
 * @brief This program read the results of
 * the circular buffer and outputs the best to stdout.
 * The solution generated are minimum arc sets
 * @version 1
 * @date 2022-11-10
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "cbuffer.h"
#include "common.h"
#include "log.h"
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int shm_fd = -1;
struct cbuffer* cbuffer = NULL;

void get_solution(struct solution* solution);
void print_solution(struct solution* s);
void terminate_generators(void);
void handle_signal(int signal);

void init_shm(void);
void init_sem(void);
void clean_shm(void);
void clean_sem(void);
void clean_exit(int exit_status);

sem_t* sem_mutex = NULL;
sem_t* sem_used = NULL;
sem_t* sem_free = NULL;

struct solution* s = NULL;

/**
 * @brief Starting point of the program supervisor.
 *
 * @param argc argument count
 * @param argv argument vector
 * @return int exit status
 */
int main(int argc, char** argv)
{
    prg_name = argv[0];

    init_shm();
    init_sem();

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_signal;
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);

    // init buffer pointers
    cbuffer->write_pos = 0;
    cbuffer->read_pos = 0;
    cbuffer->interrupt = 0;
    memset(cbuffer->solutions, 0, sizeof(struct solution) * MAX_DATA);

    int best_solutioin = -1;
    s = malloc(sizeof(struct solution));

    while (1) {
        get_solution(s);
        if (s->size == 0) {
            printf("The graph is acyclic!\n");
            terminate_generators();
            clean_exit(EXIT_SUCCESS);
        }

        if (best_solutioin == -1) {
            best_solutioin = s->size;
            continue;
        }

        if (s->size < best_solutioin) {
            best_solutioin = s->size;
            print_solution(s);
        }
    }

    clean_exit(EXIT_SUCCESS);
    return 0;
}

/**
 * @brief Handles signal SIGINT and SIGTERM.
 * It indicates the generators to terminate and
 * freees all the allocated ressources and exits.
 *
 * @param signal received signal
 */
void handle_signal(int signal)
{
    terminate_generators();
    clean_exit(EXIT_SUCCESS);
}

/**
 * @brief It indicates the generators to terminate
 */
void terminate_generators(void) { cbuffer->interrupt = 1; }

/**
 * @brief Prints the solution in a nice an readable way
 *
 * @param s solution
 */
void print_solution(struct solution* s)
{
    if (s->size < 0)
        return;
    printf("Solution with %d edges: ", s->size);
    int i;
    for (i = 0; i < s->size - 1; i++) {
        printf("%hu-%hu, ", s->edges[i].from, s->edges[i].to);
    }
    printf("%hu-%hu\n", s->edges[s->size - 1].from, s->edges[s->size - 1].to);
}

/**
 * @brief Gets the solution from the circular buffer and
 * copies the content to the solution given in the paramets
 *
 * @param solution initialized solution
 */
void get_solution(struct solution* solution)
{
    sem_wait(sem_used);
    struct solution* s = &cbuffer->solutions[cbuffer->read_pos];
    sem_post(sem_free);
    cbuffer->read_pos += 1;
    cbuffer->read_pos %= MAX_DATA;
    memcpy(solution, s, sizeof(struct solution));
}

/**
 * @brief Creates the semaphores for operating the circular buffer.
 * If a semaphore already exists an error is thrown
 *
 */
void init_sem(void)
{
    if ((sem_mutex = sem_open(SEM_MUTEX, O_CREAT | O_EXCL, 0600, 1)) == SEM_FAILED) {
        log_error("Creating semaphore mutex failed %s", strerror(errno));
        clean_exit(EXIT_FAILURE);
    }

    if ((sem_used = sem_open(SEM_USED, O_CREAT | O_EXCL, 0600, 0)) == SEM_FAILED) {
        log_error("Creating semaphore used failed %s", strerror(errno));
        clean_exit(EXIT_FAILURE);
    }

    if ((sem_free = sem_open(SEM_FREE, O_CREAT | O_EXCL, 0600, MAX_DATA)) == SEM_FAILED) {
        log_error("Creating semaphore free failed %s", strerror(errno));
        clean_exit(EXIT_FAILURE);
    }
}

/**
 * @brief Frees all the allocated memory, cleans up the ressourecs
 * and exits with the given status code.
 *
 * @param exit_status Exit status
 */
void clean_exit(int exit_status)
{
    if (s != NULL) {
        free(s);
    }
    clean_shm();
    clean_sem();
    exit(exit_status);
}

/**
 * @brief Initializes the shared mamory and maps the circular buffer
 * to the global variable cbuffer.
 *
 */
void init_shm(void)
{
    if ((shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0600)) < 0) {
        log_error("Creating shared memory failed: %s", strerror(errno));
    }

    if (ftruncate(shm_fd, sizeof(struct cbuffer)) < 0) {
        log_error("Truncate failed: %s\n", strerror(errno));
    }

    cbuffer = mmap(NULL, sizeof(*cbuffer), PROT_READ | PROT_WRITE, MAP_SHARED,
        shm_fd, 0);

    if (cbuffer == MAP_FAILED) {
        log_error("Failed to Map shared memory: %s", strerror(errno));
    }
}

/**
 * @brief Tries to close and unlink all semaphores.
 *
 */
void clean_sem(void)
{
    if (sem_free != NULL && sem_close(sem_free) < 0) {
        log_error("Closing semaphore free failed %s", strerror(errno));
    }

    if (sem_used != NULL && sem_close(sem_used) < 0) {
        log_error("Closing semaphore used failed %s", strerror(errno));
    }

    if (sem_mutex != NULL && sem_close(sem_mutex) < 0) {
        log_error("Closing semaphore mutex failed %s", strerror(errno));
    }

    if (sem_unlink(SEM_MUTEX) < 0) {
        log_error("Unlinking semaphore mutex failed %s", strerror(errno));
    }

    if (sem_unlink(SEM_FREE) < 0) {
        log_error("Unlinking semaphore free failed %s", strerror(errno));
    }

    if (sem_unlink(SEM_USED) < 0) {
        log_error("Unlinking semaphore used failed %s", strerror(errno));
    }
}

/**
 * @brief Unmaps, closes and unmaps the shared memory
 *
 */
void clean_shm(void)
{
    if (cbuffer != NULL && munmap(cbuffer, sizeof(*cbuffer)) < 0) {
        log_error("Unmapping shared memory failed: %s", strerror(errno));
    }

    if (shm_fd != -1 && close(shm_fd) < 0) {
        log_error("Closing shared memory file descriptor failed: %s",
            strerror(errno));
    }

    if (shm_unlink(SHM_NAME) < 0) {
        log_error("Failed to unlink shared memory: %s", strerror(errno));
    }
}