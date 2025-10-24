#include"graphs.h"
#include"uint32_arrays.h"

void search_nearby_vertexes_2th(uint16_t* smallest_path_v, struct priority_queue* pq, struct graph* graph, uint32_t** distances, uint8_t forward);

uint32_t** dikstra_bi_2th(struct graph* graph, uint16_t start_vertex,  uint16_t end_vertex){
    
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

    uint32_t best_path_lenght = UINT32_MAX, best_path_forw, best_path_back;
    uint16_t join_vertex = start_vertex;
    uint8_t finish = 0;
    dist_f[0][start_vertex] = 0;
    dist_f[1][start_vertex] = start_vertex;
    dist_b[0][end_vertex] = 0;
    dist_b[1][end_vertex] = end_vertex;

    #pragma omp parallel sections num_threads(2) shared(best_path_lenght, join_vertex, finish, dist_f, dist_b, Pqf, Pqb, best_path_forw, best_path_back)
    {

        #pragma omp section
        {

            while(!finish && !pq_empty(Pqf)){

                uint32_t current_distance, current_best_path_back, total_best;
                uint16_t current_vertex;

                if(!pq_pop(&current_vertex, &current_distance, Pqf)){

                    #pragma omp atomic write
                        finish = 1;

                    break;

                }

                #pragma omp atomic write 
                    best_path_forw = current_distance;

                #pragma omp atomic read
                    current_best_path_back = best_path_back;

                #pragma omp atomic read
                    total_best = best_path_lenght;

                if((uint64_t)current_distance + (uint64_t)current_best_path_back >= total_best)
                    break;
                    
                #pragma omp critical(dist_back)
                    current_best_path_back = dist_b[0][current_vertex];

                if(current_best_path_back != UINT32_MAX){

                    uint64_t total_distance = (uint64_t)current_distance + (uint64_t)current_best_path_back;

                    if(total_distance < total_best){

                        #pragma omp atomic write 
                            best_path_lenght = (uint32_t)total_distance;

                        #pragma omp atomic write
                            join_vertex = current_vertex;

                    }

                }

                search_nearby_vertexes_2th(&current_vertex, Pqf, graph, dist_f, 1);

            }

        }

        #pragma omp section
        {

            while(!finish && !pq_empty(Pqb)){

                uint32_t current_distance, current_best_path_forw, total_best;
                uint16_t current_vertex;

                if(!pq_pop(&current_vertex, &current_distance, Pqb)){

                    #pragma omp atomic write
                        finish = 1;

                    break;

                }

                #pragma omp atomic write 
                    best_path_back = current_distance;

                #pragma omp atomic read
                    current_best_path_forw = best_path_forw;

                #pragma omp atomic read
                    total_best = best_path_lenght;

                if((uint64_t)current_distance + (uint64_t)current_best_path_forw >= total_best)
                    break;
                    
                #pragma omp critical(dist_forw)
                    current_best_path_forw = dist_f[0][current_vertex];

                if(current_best_path_forw != UINT32_MAX){

                    uint64_t total_distance = (uint64_t)current_distance + (uint64_t)current_best_path_forw;

                    if(total_distance < total_best){

                        #pragma omp atomic write 
                            best_path_lenght = (uint32_t)total_distance;

                        #pragma omp atomic write
                            join_vertex = current_vertex;

                    }

                }

                search_nearby_vertexes_2th(&current_vertex, Pqb, graph, dist_b, 0);

            }

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

void search_nearby_vertexes_2th(uint16_t* smallest_path_v, struct priority_queue* pq, struct graph* graph, uint32_t** distances, uint8_t forward){

    for(uint16_t i = 0; i < graph->v; i++){

        uint16_t weight = get_edge_weight(*smallest_path_v, i, graph->adj_matrix);

        if(weight == 0)
            continue;

        uint32_t current_dist;

        if(forward){

            current_dist = distances[0][*smallest_path_v];

            if(current_dist <= UINT32_MAX - weight){

                current_dist += weight;

                uint32_t now_best = distances[0][i];

                if(current_dist < now_best){

                    #pragma omp critical(dist_forw)
                    {

                        distances[0][i] = current_dist;
                        distances[1][i] = *smallest_path_v;

                    }

                    pq_push(i, current_dist, pq);

                }

            }

        } else{

            current_dist = distances[0][*smallest_path_v];

            if(current_dist <= UINT32_MAX - weight){

                current_dist += weight;

                uint32_t now_best;
                now_best = distances[0][i];

                if(current_dist < now_best){

                    #pragma omp critical(dist_back)
                    {

                        distances[0][i] = current_dist;
                        distances[1][i] = *smallest_path_v;

                    }

                    pq_push(i, current_dist, pq);

                }

            }

        }

    }

}
