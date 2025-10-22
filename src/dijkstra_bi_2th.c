#include"graphs.h"
#include"uint32_arrays.h"

void search_nearby_vertexes_2th(uint16_t* smallest_path_v, struct priority_queue* pq, struct graph* graph, uint32_t** distances);

uint32_t** dikstra_bi_2th(struct graph* graph, uint16_t start_vertex,  uint16_t end_vertex){

    uint32_t** result = alloc_uint32_array(graph->v, "result\0");
    if(!result)
        return NULL;

    if(start_vertex == end_vertex){

        result[0][start_vertex] = 0;
        result[1][start_vertex] = end_vertex;
        return result;

    }

    uint32_t** dist_f, **dist_b;
    struct priority_queue* Pqf, *Pqb;

    #pragma omp parallel sections num_threads(2)
    {
        #pragma omp section
        {

            dist_f = alloc_uint32_array(graph->v, "dist_f\0");

            Pqf = pq_create(graph->e);


        }

        #pragma omp section
        {

            dist_b = alloc_uint32_array(graph->v, "dist_b\0");

            Pqb = pq_create(graph->e);

        }

    }

    if(!dist_f){

        free_uint32_array(result);
        free_uint32_array(dist_b);
        pq_free(Pqb);
        pq_free(Pqf);
        return NULL;

    }
    if(!dist_b){

        free_uint32_array(result);
        free_uint32_array(dist_f);
        pq_free(Pqb);
        pq_free(Pqf);
        return NULL;

    }
    if(!Pqf){

        free_uint32_array(result);
        free_uint32_array(dist_f);
        free_uint32_array(dist_b);
        pq_free(Pqb);
        return NULL;

    }
    if(!Pqb){

        free_uint32_array(result);
        free_uint32_array(dist_f);
        free_uint32_array(dist_b);
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

    // omp_lock_t 

    while(!pq_empty(Pqf) || !pq_empty(Pqb)){

        uint32_t best_path_forward = pq_peek_top_distance(Pqf);
        uint32_t best_path_backward = pq_peek_top_distance(Pqb);

        if((uint64_t)best_path_forward + (uint64_t)best_path_backward >= (uint64_t)best_path_lenght)
            break;

        uint16_t current_vertex_f, current_vertex_b;
        uint8_t end = 0;

        #pragma omp parallel sections num_threads(2)
        {

            #pragma omp section
            {

                if(!pq_pop(&current_vertex_f, &best_path_forward, Pqf)){
                    end = 1;
                } else{

                    uint32_t distance_b = dist_b[0][current_vertex_f];

                    if(distance_b != UINT32_MAX){

                        uint64_t total_dist = (uint64_t)best_path_forward + (uint64_t)distance_b;

                        if(total_dist <  best_path_lenght){

                            best_path_lenght = (uint32_t)total_dist;
                            join_vertex = current_vertex_f;

                        }

                    }

                    search_nearby_vertexes_2th(&current_vertex_f, Pqf, graph, dist_f);

                }

            }

            #pragma omp section
            {

                if(!pq_pop(&current_vertex_b, &best_path_backward, Pqb)){
                    end = 1;
                } else{

                    uint32_t distance_f = dist_f[0][current_vertex_b];

                    if(distance_f != UINT32_MAX){

                        uint64_t total_dist = (uint64_t)best_path_backward + (uint64_t)distance_f;

                        if(total_dist <  best_path_lenght){

                            best_path_lenght = (uint32_t)total_dist;
                            join_vertex = current_vertex_b;

                        }

                    }

                    search_nearby_vertexes_2th(&current_vertex_b, Pqb, graph, dist_b);

                }

            }

        }

        if(end)
            break;

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

void search_nearby_vertexes_2th(uint16_t* smallest_path_v, struct priority_queue* pq, struct graph* graph, uint32_t** distances){

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
