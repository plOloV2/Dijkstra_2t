#include"libs.h"
#include"get_data.h"
#include"dikstra.h"

#define THREADS_NUM 2

int main(int argc, char** argv){

    struct graph* data = NULL;

    uint16_t start_vertex = atoi(argv[1]);

    switch (argc){
    case 3:
        data = get_data_from_file(argv[2]);
        break;
    
    case 5:
        data = get_random_data(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));
        break;

    default:
        printf("Incorrect args were passed.\n");
        return 1;
    }

    if(!data){

        printf("ERROR: Creating data failed\n");
        return 2;

    }

    double time = omp_get_wtime();
        
    uint32_t** result = dikstra_multi(data, start_vertex, THREADS_NUM);

    time = omp_get_wtime() - time;

    printf("Dijkstra algorithm on %i threads took: %f ms.\n\nResults:\n", THREADS_NUM, time);
    for(uint16_t i = 0; i < data->v; i++){

        if(i == start_vertex){
            printf("Vertex: %i - Starting vertex\n", i+1);
        } else if(result[0][i] == 0 && result[1][i] == 0){
            printf("Vertex: %i - Disconnected form graph\n", i+1);
        } else {
            printf("Vertex: %i - Shortest path: %i Previouse vertex: %i\n", i+1, result[0][i], result[1][i]+1);
        }
        
    }
    
    for(uint16_t i = 1; i < data->v; i++)
        free(data->adj_matrix[i - 1]);
    free(data->adj_matrix);
    free(data);
    data = NULL;

    free(result);
    result=NULL;

    return 0;
    
}
