#include"graphs.h"
#include"uint32_arrays.h"

void search_nearby_vertexes_2th(uint16_t* smallest_path_v, struct priority_queue* pq, struct graph* graph, uint32_t** distances, omp_lock_t* dist_lock);

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

    omp_lock_t dist_f_lock, dist_b_lock, best_path_lock;
    uint16_t join_vertex = start_vertex;
    uint32_t best_path_lenght = UINT32_MAX, best_path_forward = 0, best_path_backward = 0;

    omp_init_lock(&dist_f_lock);
    omp_init_lock(&dist_b_lock);
    omp_init_lock(&best_path_lock);

    #pragma omp parallel sections num_threads(2) shared(dist_f,dist_b,best_path_lenght,join_vertex, best_path_forward, best_path_backward)
    {

        #pragma omp section
        {

            struct priority_queue* pqf = pq_create(graph->e);
            if(!pqf){}
                
            pq_push(start_vertex, 0, pqf);

            omp_set_lock(&dist_f_lock);
            dist_f[0][start_vertex] = 0;
            dist_f[1][start_vertex] = start_vertex;
            omp_unset_lock(&dist_f_lock);

            while(1){

                omp_set_lock(&best_path_lock);
                best_path_forward = pq_peek_top_distance(pqf);
                if((uint64_t)best_path_forward + (uint64_t)best_path_backward >= (uint64_t)best_path_lenght){

                    omp_unset_lock(&best_path_lock);
                    break;

                }
                omp_unset_lock(&best_path_lock);

                uint16_t current_vertex;
                uint32_t best_path_forward_local;
                if(!pq_pop(&current_vertex, &best_path_forward_local, pqf))
                    break;

                omp_set_lock(&dist_b_lock);
                uint32_t distance_b = dist_b[0][current_vertex];
                omp_unset_lock(&dist_b_lock);

                if(distance_b != UINT32_MAX){

                    uint64_t total_dist = (uint64_t)best_path_forward_local + (uint64_t)distance_b;

                    omp_set_lock(&best_path_lock);
                    if(total_dist < best_path_lenght){

                        best_path_lenght = (uint32_t)total_dist;
                        join_vertex = current_vertex;

                    }
                    omp_unset_lock(&best_path_lock);

                }

                search_nearby_vertexes_2th(&current_vertex, pqf, graph, dist_f, &dist_f_lock);

            }

            pq_free(pqf);

        }

        #pragma omp section
        {

            struct priority_queue* pqb = pq_create(graph->e);
            if(!pqb){}
            
            pq_push(end_vertex, 0, pqb);

            omp_set_lock(&dist_b_lock);
            dist_b[0][end_vertex] = 0;
            dist_b[1][end_vertex] = end_vertex;
            omp_unset_lock(&dist_b_lock);

            while(1){

                omp_set_lock(&best_path_lock);
                best_path_backward = pq_peek_top_distance(pqb);
                if((uint64_t)best_path_forward + (uint64_t)best_path_backward >= (uint64_t)best_path_lenght){

                    omp_unset_lock(&best_path_lock);
                    break;

                }
                omp_unset_lock(&best_path_lock);

                uint16_t current_vertex;
                uint32_t best_path_backward_local;
                if(!pq_pop(&current_vertex, &best_path_backward_local, pqb))
                    break;

                omp_set_lock(&dist_f_lock);
                uint32_t distance_f = dist_f[0][current_vertex];
                omp_unset_lock(&dist_f_lock);

                if(distance_f != UINT32_MAX){

                    uint64_t total_dist = (uint64_t)best_path_backward_local + (uint64_t)distance_f;

                    omp_set_lock(&best_path_lock);
                    if(total_dist < best_path_lenght){

                        best_path_lenght = (uint32_t)total_dist;
                        join_vertex = current_vertex;

                    }
                    omp_unset_lock(&best_path_lock);

                }

                search_nearby_vertexes_2th(&current_vertex, pqb, graph, dist_b, &dist_b_lock);

            }

            pq_free(pqb);

        }

    }

    omp_destroy_lock(&dist_f_lock);
    omp_destroy_lock(&dist_b_lock);
    omp_destroy_lock(&best_path_lock);

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

void search_nearby_vertexes_2th(uint16_t* smallest_path_v, struct priority_queue* pq, struct graph* graph, uint32_t** distances, omp_lock_t* dist_lock){

    for(uint16_t i = 0; i < graph->v; i++){

        uint16_t weight = get_edge_weight(*smallest_path_v, i, graph->adj_matrix);

        if(weight == 0)
            continue;

        omp_set_lock(dist_lock);
        uint32_t temp_dist = distances[0][*smallest_path_v];
        omp_unset_lock(dist_lock);

        if(temp_dist <= UINT32_MAX - weight){

            uint32_t new_distance = temp_dist + weight;

            omp_set_lock(dist_lock);
            uint32_t temp_dist = distances[0][i];
            
            if(new_distance < temp_dist){

                distances[0][i] = new_distance;
                distances[1][i] = *smallest_path_v;
                omp_unset_lock(dist_lock);

                pq_push(i, new_distance, pq);

            } else{

                omp_unset_lock(dist_lock);

            }

        }

    }

}
