#include"libs.h"
#include"get_data.h"
#include"dikstra.h"
#include"uint32_arrays.h"

struct outdata{

    double res1;
    double res2;
    uint16_t len1;
    uint16_t len2;
    uint32_t path_length1;
    uint32_t path_length2;

};

void progress_bar_bench(uint8_t percentage);

void run_benchmark(uint16_t V, uint8_t per_E){

    if(V < 3){

        printf("ERROR: V to small\n");
        return;

    }

    struct graph* data = NULL;
    struct outdata* output = calloc(100, sizeof(struct outdata));
    if(!output){

        printf("ERROR: outdata alloc fail\n");
        return;

    }
        

    for(uint8_t i = 0; i < 100; i++){

        data = get_random_data(V, per_E, 0, 0);
        if(!data){

            printf("ERROR: bench data creation failed\n");
            return;

        }

        output[i].res1 = omp_get_wtime();
        uint32_t** result_1th = dikstra_bi_1th(data, 0, V/2);
        output[i].res1 = omp_get_wtime() - output[i].res1;

        output[i].res2 = omp_get_wtime();
        uint32_t** result_2th = dikstra_bi_2th(data, 0, V/2);
        output[i].res2 = omp_get_wtime() - output[i].res2;

        if(result_1th[0][V/2] != result_2th[0][V/2]){

            output[i].res1 = 0;
            output[i].res2 = 0;
            i--;

        } else{

            output[i].res1 *= 1000;
            output[i].res2 *= 1000;

            output[i].path_length1 = result_1th[0][V/2];
            output[i].path_length2 = result_2th[0][V/2];

            if(output[i].path_length1 != output[i].path_length2)
                printf("miss\n");

            for(uint16_t j = 0; j < V; j++){

                if(result_1th[0][j] != UINT32_MAX)
                    output[i].len1++;

                if(result_2th[0][j] != UINT32_MAX)
                    output[i].len2++;

            }

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

    fprintf(file, "i;time_1th;time_2th;length1;length2;path1;path2\n");

    for(uint8_t i = 0; i < 100; i++)
        fprintf(file, "%d;%.9f;%.9f;%hu;%hu;%u;%u\n", (int)i, output[i].res1, output[i].res2, output[i].len1, output[i].len2, output[i].path_length1, output[i].path_length2);

    fclose(file);

    free(output);

    printf("Calculations done and saved to: %s\n\n", filename);

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
