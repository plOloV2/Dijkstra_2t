#include"graphs.h"

uint32_t** dikstra_single(struct graph* graph, uint16_t start_vertex){

    uint32_t** result = malloc(sizeof(uint32_t*)*2);

    if(!result){

        printf("ERROR: Dijkstra single result** malloc failed\n");
        return NULL;

    }

    result[0] = malloc(graph->v * sizeof(uint32_t));
    result[1] = malloc(graph->v * sizeof(uint32_t));
    if(!result[0] || !result[1]){

        printf("ERROR: Dijkstra single result* malloc failed\n");
        free(result);
        return NULL;

    }

    memset(result[0], 0xff, graph->v * sizeof(uint32_t));
    memset(result[1], 0xff, graph->v * sizeof(uint32_t));

    struct priority_queue* pq = pq_create(graph->e);
    pq_push(start_vertex, 0, pq);

    result[0][start_vertex] = 0;

    while(!pq_empty(pq)){

        uint32_t smallest_path ;
        uint16_t smallest_path_v ;

        if(!pq_pop(&smallest_path_v, &smallest_path, pq))
            break;

        if(smallest_path > result[0][smallest_path_v])
            continue;

        for(uint16_t i = 0; i < graph->v; i++){

            uint16_t weight = get_edge_weight(smallest_path_v, i, graph->adj_matrix);

            if(weight == 0)
                continue;

            if(result[0][smallest_path_v] <= UINT32_MAX - weight){

                uint32_t new_distance = result[0][smallest_path_v] + weight;
                
                if(new_distance < result[0][i]){

                    result[0][i] = new_distance;
                    result[1][i] = smallest_path_v;
                    pq_push(i, new_distance, pq);

                }

            }

        }

    }

    pq_free(pq);
    return result;

}
