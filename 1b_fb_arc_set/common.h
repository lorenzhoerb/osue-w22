/**
 * @file common.h
 * @author Lorenz Hörburger (12024737)
 * @brief This header defines a the common
 * stuct edge
 * @version 0.1
 * @date 2022-11-10
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef COMMON
#define COMMON

#include <stdint.h>

typedef struct {
    uint16_t from;
    uint16_t to;
} edge_t;

#endif
