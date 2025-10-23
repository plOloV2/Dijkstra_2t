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
    uint32_t best_path_lenght = UINT32_MAX;
    uint16_t join_vertex = start_vertex;
    uint8_t end_loop = 0, alloc_fail = 0;;
    omp_lock_t end_loop_lock, dist_f_lock, dist_b_lock, join_vertex_lock;

    #pragma omp parallel num_threads(2)
    {

        #pragma omp sections
        {

            #pragma omp section
            {

                dist_f = alloc_uint32_array(graph->v, "dist_f\0");

                if(dist_f != NULL)
                    Pqf = pq_create(graph->e);


            }

            #pragma omp section
            {

                dist_b = alloc_uint32_array(graph->v, "dist_b\0");

                if(dist_b != NULL)
                    Pqb = pq_create(graph->e);

            }

        }

        #pragma omp master
        {
    
            if(!dist_f){

                free_uint32_array(result);
                free_uint32_array(dist_b);
                pq_free(Pqb);
                alloc_fail = 1;

            } else if(!dist_b){

                free_uint32_array(result);
                free_uint32_array(dist_f);
                pq_free(Pqf);
                alloc_fail = 1;

            } else if(!Pqf){

                free_uint32_array(result);
                free_uint32_array(dist_f);
                free_uint32_array(dist_b);
                pq_free(Pqb);
                alloc_fail = 1;

            }else if(!Pqb){

                free_uint32_array(result);
                free_uint32_array(dist_f);
                free_uint32_array(dist_b);
                pq_free(Pqf);
                alloc_fail = 1;
                
            }

            if(!alloc_fail){

                omp_init_lock(&end_loop_lock);
                omp_init_lock(&dist_f_lock);
                omp_init_lock(&dist_b_lock);
                omp_init_lock(&join_vertex_lock);

                pq_push(start_vertex, 0, Pqf);
                pq_push(end_vertex, 0, Pqb);

                dist_f[0][start_vertex] = 0;
                dist_f[1][start_vertex] = start_vertex;
                dist_b[0][end_vertex] = 0;
                dist_b[1][end_vertex] = end_vertex;

                end_loop = !(!pq_empty(Pqf) || !pq_empty(Pqb));

            }
        
        }

        #pragma omp barrier

        while(!end_loop && !alloc_fail){

            uint32_t best_path_forward, best_path_backward;

            #pragma omp master
                if((uint64_t)pq_peek_top_distance(Pqf) + (uint64_t)pq_peek_top_distance(Pqb) >= (uint64_t)best_path_lenght)
                    end_loop = 1;
            
            #pragma omp barrier

            uint16_t current_vertex_f, current_vertex_b;

            #pragma omp sections
            {

                #pragma omp section
                {

                    if(!pq_pop(&current_vertex_f, &best_path_forward, Pqf)){
                        omp_set_lock(&end_loop_lock);
                        end_loop = 1;
                        omp_unset_lock(&end_loop_lock);
                    } else{

                        omp_set_lock(&dist_b_lock);
                        uint32_t distance_b = dist_b[0][current_vertex_f];
                        omp_unset_lock(&dist_b_lock);

                        if(distance_b != UINT32_MAX){

                            uint64_t total_dist = (uint64_t)best_path_forward + (uint64_t)distance_b;

                            omp_set_lock(&join_vertex_lock);
                            if(total_dist <  best_path_lenght){

                                best_path_lenght = (uint32_t)total_dist;
                                join_vertex = current_vertex_f;

                            }
                            omp_unset_lock(&join_vertex_lock);

                        }

                        omp_set_lock(&dist_f_lock);
                        search_nearby_vertexes_2th(&current_vertex_f, Pqf, graph, dist_f);
                        omp_unset_lock(&dist_f_lock);

                    }

                }

                #pragma omp section
                {

                    if(!pq_pop(&current_vertex_b, &best_path_backward, Pqb)){
                        omp_set_lock(&end_loop_lock);
                        end_loop = 1;
                        omp_unset_lock(&end_loop_lock);
                    } else{

                        omp_set_lock(&dist_f_lock);
                        uint32_t distance_f = dist_f[0][current_vertex_b];
                        omp_unset_lock(&dist_f_lock);

                        if(distance_f != UINT32_MAX){

                            uint64_t total_dist = (uint64_t)best_path_backward + (uint64_t)distance_f;

                            omp_set_lock(&join_vertex_lock);
                            if(total_dist <  best_path_lenght){

                                best_path_lenght = (uint32_t)total_dist;
                                join_vertex = current_vertex_b;

                            }
                            omp_unset_lock(&join_vertex_lock);

                        }

                        omp_set_lock(&dist_b_lock);
                        search_nearby_vertexes_2th(&current_vertex_b, Pqb, graph, dist_b);
                        omp_unset_lock(&dist_b_lock);

                    }

                }

            }

        }

    }
    
    omp_destroy_lock(&end_loop_lock);
    omp_destroy_lock(&dist_f_lock);
    omp_destroy_lock(&dist_b_lock);
    omp_destroy_lock(&join_vertex_lock);

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
