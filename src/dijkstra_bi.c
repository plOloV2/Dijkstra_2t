#include"graphs.h"

uint32_t** dikstra_multi(struct graph* graph, uint16_t start_vertex, uint8_t num_of_threads){

    uint32_t** result = malloc(sizeof(uint32_t*)*2);

    if(!result){

        printf("ERROR: Dijkstra bidirect result** malloc failed\n");
        return NULL;

    }

    result[0] = malloc(graph->v * sizeof(uint32_t));
    result[1] = malloc(graph->v * sizeof(uint32_t));
    if(!result[0] || !result[1]){

        printf("ERROR: Dijkstra bidirect result* malloc failed\n");
        free(result);
        return NULL;

    }

    return result;

}
