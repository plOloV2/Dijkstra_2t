#ifndef DIKSTRA
#define DIKSTRA

#include"libs.h"
#include"graph.h"

uint32_t** dikstra_bi_1th(struct graph* graph, uint16_t start_vertex,  uint16_t end_vertex);

uint32_t** dikstra_bi_2th(struct graph* graph, uint16_t start_vertex,  uint16_t end_vertex);

#endif
