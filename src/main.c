#include"libs.h"
#include"get_data.h"
#include"dikstra.h"
#include"uint32_arrays.h"

void run_benchmark(uint16_t V, uint8_t per_E);

int main(int argc, char** argv){

    struct graph* data = NULL;

    switch (argc){
        case 3:
            run_benchmark(atoi(argv[1]), atoi(argv[2]));
            return 0;

        case 4:
            data = get_data_from_file(argv[3]);
            break;
        
        case 6:
            data = get_random_data(atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), 1);
            break;

        default:
            printf("Incorrect args were passed.\n");
            return 1;
    }

    uint16_t start_vertex = atoi(argv[1]);
    if(start_vertex == 0 || start_vertex > data->v){
        printf("Vertex index starts from 1 and cannot be bigger than number of vertexes\n");
        return 3;
    }    
    uint16_t end_vertex = atoi(argv[2]);
    if(end_vertex == 0 || end_vertex > data->v){
        printf("Vertex index starts from 1 and cannot be bigger than number of vertexes\n");
        return 4;
    }

    start_vertex--;
    end_vertex--;

    if(!data){

        printf("ERROR: Creating data failed\n");
        return 2;

    }

    printf("\n\nStarting calculations...\n");

    double time_1th = omp_get_wtime();
        
    uint32_t** result_1th = dikstra_bi_1th(data, start_vertex, end_vertex);

    time_1th = omp_get_wtime() - time_1th;

    printf("Bidirectional Dijkstra algorithm on 1 thread took: %f ms.\n", time_1th*1000);

    double time_2th = omp_get_wtime();
        
    uint32_t** result_2th = dikstra_bi_2th(data, start_vertex, end_vertex);

    time_2th = omp_get_wtime() - time_2th;

    printf("Bidirectional Dijkstra algorithm on 2 threads took: %f ms.\n", time_2th*1000);

    if(!compare_uint32_arrays(result_1th, result_2th, data->v)){

        printf("Results missmatch!\n\n");

        for(uint16_t i = 0; i < data->v; i++){

            if(i == start_vertex){
                printf("Vertex: %hu - Starting vertex\n", i+1);
            } else if(i == end_vertex){
                printf("Vertex: %hu - End vertex, Shortest path: %u Previouse vertex: %i\n", i+1, result_1th[0][i], result_1th[1][i]+1);
            } else if(result_1th[0][i] != UINT32_MAX){
                printf("Vertex: %hu - Shortest path: %u Previouse vertex: %u\n", i+1, result_1th[0][i], result_1th[1][i]+1);
            }
            
        }

        printf("\n\nWyniki 2 watki\n\n");

        for(uint16_t i = 0; i < data->v; i++){

            if(i == start_vertex){
                printf("Vertex: %hu - Starting vertex\n", i+1);
            } else if(i == end_vertex){
                printf("Vertex: %hu - End vertex, Shortest path: %u Previouse vertex: %i\n", i+1, result_2th[0][i], result_2th[1][i]+1);
            } else if(result_2th[0][i] != UINT32_MAX){
                printf("Vertex: %hu - Shortest path: %u Previouse vertex: %u\n", i+1, result_2th[0][i], result_2th[1][i]+1);
            }
            
        }

    }
        
    if(result_1th[0][end_vertex] == UINT32_MAX)
        printf("Path between %hu and %hu does not exist\n", start_vertex, end_vertex);

    
    
    for(uint16_t i = 1; i < data->v; i++)
        free(data->adj_matrix[i - 1]);
    free(data->adj_matrix);
    free(data);
    data = NULL;

    free_uint32_array(result_1th);
    result_1th=NULL;
    free_uint32_array(result_2th);
    result_2th=NULL;

    return 0;
    
}
