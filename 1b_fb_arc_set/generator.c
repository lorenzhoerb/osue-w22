/**
 * @file generator.c
 * @author Lorenz Hörburger 12024737
 * @brief Takes arguments and interprets them as edges of a graph.
 * A heuristic arcset solution is prduced and written to the circular buffer.
 * @version 0.1
 * @date 2022-11-10
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "cbuffer.h"
#include "common.h"
#include "graph.h"
#include "log.h"
#include <errno.h>
#include <fcntl.h>
#include <regex.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#define REG_EDGE_PATTERN "^[0-9]+-[0-9]+$"

void parse_edge(const char* edge_str, edge_t* edge);
void parse_graph(int argc, char** argv, graph_t* graph);

void gen_arcset(graph_t* graph, struct solution* arcset);

void init_sem(void);
void init_reg_edge(void);
void init_shm(void);

void clean_shm(void);
void clean_sem(void);

void clean_exit(int exit_status);

void set_random_seed(void);
void usage(void);

void write_buffer(struct solution* solution);

regex_t reg_edge;

int shm_fd = -1;
struct cbuffer* cbuffer = NULL;

sem_t* sem_mutex = NULL;
sem_t* sem_used = NULL;
sem_t* sem_free = NULL;

/**
 * @brief Starting point of the program generator
 *
 * @param argc argument count
 * @param argv argument vector
 * @return int return status
 */
int main(int argc, char** argv)
{
    prg_name = argv[0];
    init_reg_edge();
    set_random_seed();

    int edge_count = argc - 1;
    edge_t edges[edge_count];
    uint16_t vertecies[edge_count * 2];
    graph_t graph = { .e_size = edge_count,
        .edges = edges,
        .v_size = 0,
        .vertecies = vertecies };

    parse_graph(argc, argv, &graph);
    struct solution arcset;

    init_shm();
    init_sem();

    while (1) {
        // exit superviser interrupted
        if (cbuffer->interrupt) {
            clean_exit(EXIT_SUCCESS);
        }

        arcset.size = 0;
        memset(&arcset.edges, 0, sizeof(edge_t) * MAX_EDGES);

        gen_arcset(&graph, &arcset);

        if (arcset.size < MAX_EDGES) {
            write_buffer(&arcset);
        }
    }

    clean_exit(EXIT_SUCCESS);
    return 0;
}

/**
 * @brief Writes the given solution to the shared memory
 *
 * @param solution arcset solution
 */
void write_buffer(struct solution* solution)
{
    sem_wait(sem_free);
    sem_wait(sem_mutex);
    struct solution* s = &cbuffer->solutions[cbuffer->write_pos];
    memcpy(s, solution, sizeof(struct solution));
    cbuffer->write_pos += 1;
    cbuffer->write_pos %= MAX_DATA;
    sem_post(sem_mutex);
    sem_post(sem_used);
}

/**
 * @brief Generates an arcset out of the given graph and
 * sets the result to the given arcset struct
 *
 * @param graph Initialized graph with edges and vertecies
 * @param arcset Initialized arcset struct
 */
void gen_arcset(graph_t* graph, struct solution* arcset)
{
    shuffle_vertecies(graph);
    int i;
    for (i = 0; i < graph->e_size; i++) {
        edge_t edge = graph->edges[i];
        if (edge_selected(graph, i)) {
            printf("%hu-%hu, ", edge.from, edge.to);
            arcset->edges[arcset->size++] = edge;
        }
    }
    printf("\n");
}

/**
 * @brief Tries to parse the given arguments to an graph and sets it to
 * to the given graph argument. At least one edge must be given.
 *
 * @param argc argument count
 * @param argv argument vector
 * @param graph initialized graph
 */
void parse_graph(int argc, char** argv, graph_t* graph)
{
    if (argc < 2) {
        log_error("Invalid arguments. At least one argument (edge) needed");
        usage();
        clean_exit(EXIT_FAILURE);
    }

    int i;
    for (i = 1; i < argc; i++) {
        edge_t edge;
        parse_edge(argv[i], &edge);
        graph->edges[i - 1] = edge;
        add_vertex(graph, edge.from);
        add_vertex(graph, edge.to);
    }
}

/**
 * @brief Tries to parse an edge string e.g: 1-2
 * to an edge object. If the edge string has an invalid
 * format the methode throws an parsing error an terminates
 * the program. The parsed edge gets saved in the given edges argument.
 *
 * @param edge_str Edge string
 * @param edge Edge where the parsed edge is saved to
 */
void parse_edge(const char* edge_str, edge_t* edge)
{
    if (regexec(&reg_edge, edge_str, 0, NULL, 0)) {
        log_error("Parsing edge failed. Invalid format: %s", edge_str);
        exit(EXIT_FAILURE);
    }

    if (sscanf(edge_str, "%hu-%hu", &(edge->from), &(edge->to)) < 2) {
        log_error("Parsing edge failed. Invalid format: %s", edge_str);
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Opens and maps the circular buffer.
 * Throws an error if an error occured
 *
 */
void init_shm(void)
{
    if ((shm_fd = shm_open(SHM_NAME, O_RDWR, 0600)) < 0) {
        log_error("Open shared memory failed: %s", strerror(errno));
        clean_exit(EXIT_FAILURE);
    }

    cbuffer = mmap(NULL, sizeof(*cbuffer), PROT_READ | PROT_WRITE, MAP_SHARED,
        shm_fd, 0);

    if (cbuffer == MAP_FAILED) {
        log_error("Failed to map shared memory: %s", strerror(errno));
        clean_exit(EXIT_FAILURE);
    }
}

/**
 * @brief Cleans up the shared memory. Unmaps and closes it
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
}

/**
 * @brief Tries to clean the open semaphores of the circular buffer
 *
 */
void clean_sem(void)
{
    if (sem_free != NULL && sem_close(sem_free) < 0) {
        log_error("Closing semaphore free failed: %s", strerror(errno));
    }

    if (sem_used != NULL && sem_close(sem_used) < 0) {
        log_error("Closing semaphore used failed: %s", strerror(errno));
    }

    if (sem_mutex != NULL && sem_close(sem_mutex) < 0) {
        log_error("Closing semaphore mutex failed: %s", strerror(errno));
    }
}

/**
 * @brief Cleans up all the resources and
 * exits with the given exit status.
 *
 * @param exit_status
 */
void clean_exit(int exit_status)
{
    clean_shm();
    clean_sem();
    regfree(&reg_edge);
    exit(exit_status);
}

/**
 * @brief Initializes the semaphores for the circular buffer
 *
 */
void init_sem(void)
{
    if ((sem_mutex = sem_open(SEM_MUTEX, 0)) == SEM_FAILED) {
        log_error("Opening semaphore mutex failed: %s", strerror(errno));
        clean_exit(EXIT_FAILURE);
    }

    if ((sem_used = sem_open(SEM_USED, 0)) == SEM_FAILED) {
        log_error("Opening semaphore used failed: %s", strerror(errno));
        clean_exit(EXIT_FAILURE);
    }

    if ((sem_free = sem_open(SEM_FREE, 0)) == SEM_FAILED) {
        log_error("Opening semaphore free failed: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief sets a random seed
 *
 */
void set_random_seed(void)
{
    struct timeval time;
    gettimeofday(&time, NULL);
    srand(time.tv_usec * getpid());
}

/**
 * @brief Composes the regex patter: REG_EDGE_PATTERN
 * and sets the result to reg_edge
 *
 */
void init_reg_edge(void)
{
    if (regcomp(&reg_edge, REG_EDGE_PATTERN, REG_EXTENDED)) {
        log_error("Regex compile failed for the patter: %s", REG_EDGE_PATTERN);
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Prints the usage of the program to stderr
 *
 */
void usage(void) { fprintf(stderr, "Usage: %s EDGE1...\n", prg_name); }
