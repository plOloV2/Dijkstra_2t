#include"graphs.h"
#include"uint32_arrays.h"

void search_nearby_vertexes_1th(uint16_t* smallest_path_v, struct priority_queue* pq, struct graph* graph, uint32_t** distances);

uint32_t** dikstra_bi_1th(struct graph* graph, uint16_t start_vertex,  uint16_t end_vertex){
    
    uint32_t** result = alloc_uint32_array(graph->v, "result\0");
    if(!result)
        return NULL;

    if(start_vertex == end_vertex){

        result[0][start_vertex] = 0;
        result[1][start_vertex] = end_vertex;
        return result;

    }

    uint32_t** dist_f = alloc_uint32_array(graph->v, "dist_f\0");
    uint32_t** dist_b = alloc_uint32_array(graph->v, "dist_b\0");
    if(!dist_f){

        free_uint32_array(result);
        free_uint32_array(dist_b);
        return NULL;

    }
    if(!dist_b){

        free_uint32_array(result);
        free_uint32_array(dist_f);
        return NULL;

    }

    struct priority_queue* Pqf = pq_create(graph->e);
    struct priority_queue* Pqb = pq_create(graph->e);
    if(!Pqf){

        pq_free(Pqb);
        return NULL;

    }
    if(!Pqb){

        pq_free(Pqf);
        return NULL;
        
    }

    pq_push(start_vertex, 0, Pqf);
    pq_push(end_vertex, 0, Pqb);

    uint32_t best_path_lenght = UINT32_MAX;
    uint16_t join_vertex = start_vertex;
    dist_f[0][start_vertex] = 0;
    dist_f[1][start_vertex] = start_vertex;
    dist_b[0][end_vertex] = 0;
    dist_b[1][end_vertex] = end_vertex;

    while(!pq_empty(Pqf) || !pq_empty(Pqb)){

        uint32_t best_path_forward = pq_peek_top_distance(Pqf);
        uint32_t best_path_backward = pq_peek_top_distance(Pqb);

        if((uint64_t)best_path_forward + (uint64_t)best_path_backward >= (uint64_t)best_path_lenght)
            break;

        uint16_t current_vertex;

        if(best_path_forward <= best_path_backward){

            if(!pq_pop(&current_vertex, &best_path_forward, Pqf))
                break;

            if(dist_b[0][current_vertex] != UINT32_MAX){

                uint64_t total_dist = (uint64_t)best_path_forward + (uint64_t)dist_b[0][current_vertex];

                if(total_dist <  best_path_lenght){

                    best_path_lenght = (uint32_t)total_dist;
                    join_vertex = current_vertex;

                }

            }

            search_nearby_vertexes_1th(&current_vertex, Pqf, graph, dist_f);

        }else{

            if(!pq_pop(&current_vertex, &best_path_backward, Pqb))
                break;

            if(dist_f[0][current_vertex] != UINT32_MAX){

                uint64_t total_dist = (uint64_t)best_path_backward + (uint64_t)dist_f[0][current_vertex];

                if(total_dist <  best_path_lenght){

                    best_path_lenght = (uint32_t)total_dist;
                    join_vertex = current_vertex;

                }

            }

            search_nearby_vertexes_1th(&current_vertex, Pqb, graph, dist_b);

        }

    }

    pq_free(Pqf);
    pq_free(Pqb);

    if(best_path_lenght == UINT32_MAX){

        free_uint32_array(dist_f);
        free_uint32_array(dist_b);
        return result;
    }

    result[0][start_vertex] = 0;
    uint16_t current = join_vertex;
    while(current != start_vertex){

        result[0][current] = dist_f[0][current];
        result[1][current] = dist_f[1][current];
        current = dist_f[1][current];

    }

    current = join_vertex;
    uint16_t next_in_path = dist_b[1][current];
    while(current != end_vertex){

        result[0][next_in_path] = best_path_lenght - dist_b[0][next_in_path];
        result[1][next_in_path] = current;

        current = next_in_path;
        next_in_path = dist_b[1][current];

    }

    result[0][end_vertex] = best_path_lenght;

    free_uint32_array(dist_f);
    free_uint32_array(dist_b);

    return result;

}

void search_nearby_vertexes_1th(uint16_t* smallest_path_v, struct priority_queue* pq, struct graph* graph, uint32_t** distances){

    for(uint16_t i = 0; i < graph->v; i++){

        uint16_t weight = get_edge_weight(*smallest_path_v, i, graph->adj_matrix);

        if(weight == 0)
            continue;

        if(distances[0][*smallest_path_v] <= UINT32_MAX - weight){

            uint32_t new_distance = distances[0][*smallest_path_v] + weight;
            
            if(new_distance < distances[0][i]){

                distances[0][i] = new_distance;
                distances[1][i] = *smallest_path_v;
                pq_push(i, new_distance, pq);

            }

        }

    }

}
