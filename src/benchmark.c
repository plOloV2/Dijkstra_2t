#include"libs.h"
#include"get_data.h"
#include"dikstra.h"
#include"uint32_arrays.h"

void progress_bar_bench(uint8_t percentage);

void run_benchmark(uint16_t V, uint8_t per_E){

    if(V < 3){

        printf("ERROR: V to small\n");
        return;

    }

    struct graph* data = NULL;
    double results[100][2];

    for(uint8_t i = 0; i < 100; i++){

        data = get_random_data(V, per_E, 0, 0);
        if(!data){

            printf("ERROR: bench data creation failed\n");
            return;

        }

        results[i][0] = omp_get_wtime();
        uint32_t** result_1th = dikstra_bi_1th(data, 0, V/2);
        results[i][0] = omp_get_wtime() - results[i][0];

        results[i][1] = omp_get_wtime();
        uint32_t** result_2th = dikstra_bi_2th(data, 0, V/2);
        results[i][1] = omp_get_wtime() - results[i][1];

        if(!compare_uint32_arrays(result_1th, result_2th, V)){

            // fprintf(stderr, "ERROR: results missmatch\n");
            results[i][0] = 0;
            results[i][1] = 0;
            i--;

        }

        free_uint32_array(result_1th);
        free_uint32_array(result_2th);

        for(uint16_t j = 1; j < V; j++)
            free(data->adj_matrix[j - 1]);
        free(data->adj_matrix);
        free(data);
        data = NULL;

        progress_bar_bench(i+1);

    }

    printf("\n\n");

    char filename[64];
    snprintf(filename, sizeof(filename), "result%d_%d.csv", (int)V, (int)per_E);

    FILE *file = fopen(filename, "w");
    if(file == NULL){

        fprintf(stderr, "Error opening file %s\n", filename);
        return;

    }

    fprintf(file, "i;time_1th;time_2th\n");

    for(uint8_t i = 0; i < 100; i++)
        fprintf(file, "%d;%.9f;%.9f\n", (int)i, results[i][0], results[i][1]);

    printf("Calculations done and saved to: %s\n\n\n", filename);
}

void progress_bar_bench(uint8_t percentage){

    const int bar_width = 50;
    int pos = bar_width * percentage / 100;
    
    printf("\rCalculation progress: [");
    for(int i = 0; i < bar_width; ++i){

        if(i < pos)
            printf("#");
        else if(i == pos)
            printf(">");
        else
            printf("-");
    }

    printf("] %d%%", percentage);
    fflush(stdout);

}