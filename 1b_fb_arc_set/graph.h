/**
 * @file graph.h
 * @author Lorenz Hörburger (12024737)
 * @brief This header contains methode definitions and data structurs
 * for a graph
 * @version 0.1
 * @date 2022-11-10
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef GRAPH
#define GRAPH

#include "common.h"
#include <stdlib.h>

typedef struct {
    edge_t* edges;
    size_t e_size;
    uint16_t* vertecies;
    size_t v_size;
} graph_t;

/**
 * @brief Adds a vertex to the vertex set and
 * increments v_size by one.
 *
 * @param graph Initialized graph where vertex gets added.
 * @param vertex Vertex to be added
 */
void add_vertex(graph_t* graph, uint16_t vertex);

/**
 * @brief Checks if the vertex is already existing in the vertex set
 *
 * @param graph Initialized graph
 * @param vertex Vertex to check if it is already in the vertex set.
 * @return returns 1 if vertex is already contained else 0
 */
int graph_contains_vertex(graph_t* graph, uint16_t vertex);

/**
 * @brief Swaps the vertecies with the index i and j by its position
 * int the vertecies array.
 *
 * @param vertecies vertecies.
 * @param i index of vertex 1
 * @param j index of vertex 2
 */
void exchange_vertecies(uint16_t* vertecies, int i, int j);

/**
 * @brief Suffles the vertecies in the graph
 * randomly with the Fisher–Yates shuffle algorithm.
 *
 * @param graph Initialized graph viwth containing vertecies
 */
void shuffle_vertecies(graph_t* graph);

/**
 * @brief Checks if a edge should be selected for the arcset
 *
 * @param graph initialized graph with edges and vertecies
 * @param edge_index index of edge to check
 * @return Returns 1 if edge(u,v)  u > v in the vertex index position
 */
int edge_selected(graph_t* graph, size_t edge_index);

/**
 * @brief Returns a random int i: min <= i <= max
 *
 * @param min minimum random int
 * @param max maximum random int
 * @return random integer
 */
int rand_int_between(int min, int max);

#endif
