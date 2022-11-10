/**
 * @file cbuffer.h
 * @author Lorenz Hörburger 12024737
 * @brief Sets the constants and the data structure needed for
 * the circular buffer
 * @version 0.1
 * @date 2022-11-10
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef CBUFFER
#define CBUFFER

#include "common.h"

#define MAX_DATA (20)
#define MAX_EDGES (8)
#define SHM_NAME "/12024737_cbuff"
#define SEM_MUTEX "/12024737_sem_mutex"
#define SEM_FREE "/12024737_sem_free"
#define SEM_USED "/12024737_sem_used"

struct solution {
    edge_t edges[MAX_EDGES];
    unsigned int size;
} solution_t;

struct cbuffer {
    unsigned int write_pos;
    unsigned int read_pos;
    int interrupt;
    struct solution solutions[MAX_DATA];
} cbuffer_t;

#endif
