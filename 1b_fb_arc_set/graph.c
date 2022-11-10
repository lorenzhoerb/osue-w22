/**
 * @file graph.c
 * @author Lorenz Hörnurger (120247373)
 * @brief Implementation of the basic function on the graph structure
 * @version 0.1
 * @date 2022-11-10
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int edge_selected(graph_t* graph, size_t edge_index)
{
    edge_t edge = graph->edges[edge_index];
    int i;
    for (i = 0; i < graph->v_size; i++) {
        if (graph->vertecies[i] == edge.from)
            return 1;
        if (graph->vertecies[i] == edge.to)
            return 0;
    }
    return 0;
}

void shuffle_vertecies(graph_t* graph)
{
    int i;
    for (i = 0; i < graph->v_size - 1; i++) {
        int j = rand_int_between(i, graph->v_size - 1);
        exchange_vertecies(graph->vertecies, i, j);
    }
}

void exchange_vertecies(uint16_t* vertecies, int i, int j)
{
    uint16_t tmp;
    tmp = vertecies[i];
    vertecies[i] = vertecies[j];
    vertecies[j] = tmp;
}

void add_vertex(graph_t* graph, uint16_t vertex)
{
    if (!graph_contains_vertex(graph, vertex)) {
        graph->vertecies[graph->v_size++] = vertex;
    }
}

int graph_contains_vertex(graph_t* graph, uint16_t vertex)
{
    int i;
    for (i = 0; i < graph->v_size; i++) {
        if (graph->vertecies[i] == vertex)
            return 1;
    }
    return 0;
}

int rand_int_between(int min, int max)
{
    return (rand() % (max - min + 1)) + min;
}
