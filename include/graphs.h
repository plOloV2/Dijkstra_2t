#ifndef GRAPHS
#define GRAPHS

#include"graph.h"
#include"priority_que.h"

uint16_t get_edge_weight(uint16_t v, uint16_t u, uint16_t** adj_matrix);

void write_edge_weight(uint16_t v, uint16_t u, uint16_t weight, uint16_t** adj_matrix);

#endif
