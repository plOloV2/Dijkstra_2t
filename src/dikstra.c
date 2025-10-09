#include"libs.h"
#include"graphs.h"

uint32_t** dikstra_2t(struct graph* graph, uint16_t start_vertex, uint8_t num_of_threads){

    uint32_t** result = malloc(sizeof(uint32_t*)*2);

    if(!result)
        return NULL;

    result[0] = calloc(graph->v, sizeof(uint32_t));
    result[1] = calloc(graph->v, sizeof(uint32_t));
    if(!result[0] || !result[1])
        return NULL;

    uint16_t num_visited_v = 1;
    uint8_t* visited_v = calloc(graph->v, sizeof(uint8_t));
    visited_v[start_vertex] = 1;

    omp_set_num_threads(num_of_threads);
    printf("Using %i threads\n", omp_get_num_threads());

    #pragma omp parallel for
    for(uint16_t i = 0; i < graph->v; i++){

        if(i == start_vertex)
            continue;

        uint16_t edge = get_edge_weight(start_vertex, i, graph->adj_matrix);

        if(edge == 0)
            continue;

        if(edge < result[0][i] || result[0][i] == 0){
            result[0][i] = edge;
            result[1][i] = start_vertex;
        }

    }

    while(num_visited_v < graph->v){

        uint16_t smallest_path_V;
        uint32_t smallest_path_lenght = 0xffffffff;

        for(uint16_t i = 0; i < graph->v; i++){

            if(result[1][i] != 0 && visited_v[i] == 0 && result[0][i] < smallest_path_lenght){
                smallest_path_V = i;
                smallest_path_lenght = result[0][i];
            }

        }

        #pragma omp parallel for
        for(uint16_t i = 0; i < graph->v; i++){

            if(i == smallest_path_V)
                continue;

            uint16_t edge = get_edge_weight(smallest_path_V, i, graph->adj_matrix);

            if(edge == 0)
                continue;

            if(edge + smallest_path_lenght < result[0][i] || result[0][i] == 0){

                result[0][i] = edge + smallest_path_lenght;
                result[1][i] = smallest_path_V;

            }

        }

        num_visited_v++;
        visited_v[smallest_path_V] = 1;

    }

    return result;

}