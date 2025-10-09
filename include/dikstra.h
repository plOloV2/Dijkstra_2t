#ifndef DIKSTRA
#define DIKSTRA

#include"libs.h"
#include"graph.h"

uint32_t** dikstra_2t(struct graph* graph, uint16_t start_vertex, uint8_t num_of_threads);

#endif
