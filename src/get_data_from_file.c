#include"libs.h"
#include"graphs.h"

struct graph* get_data_from_file(char* file_name){

    FILE* file = fopen(file_name, "r");
    if(!file){

        printf("Error: Could not open file %s\n", file_name);
        return NULL;

    }

    uint16_t V, E;
    if(fscanf(file, "%hu %hu", &V, &E) != 2){

        printf("Error: Could not read V and E from file\n");
        fclose(file);
        return NULL;

    }

    struct graph* g = malloc(sizeof(struct graph));

    if(g == NULL){

        printf("Error: Memory allocation failed for graph\n");
        fclose(file);
        return NULL;

    }

    g->v = V;
    g->e = E;

    g->adj_matrix = malloc((V - 1) * sizeof(uint16_t*));

    if(g->adj_matrix == NULL){

        printf("Error: Memory allocation failed for adj_matrix rows\n");
        free(g);
        fclose(file);
        return NULL;
        
    }

    for (uint16_t i = 1; i < V; i++) {

        g->adj_matrix[i - 1] = calloc(i, sizeof(uint16_t));

        if(g->adj_matrix[i - 1] == NULL){

            printf("Error: Memory allocation failed for adj_matrix[%hu]\n", i);

            for(uint16_t j = 1; j < i; j++)
                free(g->adj_matrix[j - 1]);
            
            free(g->adj_matrix);
            free(g);
            fclose(file);
            return NULL;

        }

    }

    for(uint16_t i = 0; i < E; i++){

        uint16_t u, v, weight;

        if(fscanf(file, "%hu %hu %hu", &u, &v, &weight) != 3){

            printf("Error: Could not read edge %hu from file\n", i);

            for(uint16_t j = 1; j < V; j++)
                free(g->adj_matrix[j - 1]);
            
            free(g->adj_matrix);
            free(g);
            fclose(file);
            return NULL;

        }
        
        write_edge_weight(u, v, weight, g->adj_matrix);

    }

    fclose(file);
    return g;

}
