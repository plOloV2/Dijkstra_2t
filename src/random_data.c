#include"libs.h"
#include"graphs.h"
#include<time.h>

#define EDGE_MAX_WEIGHT 0xfffd

uint16_t gcd(uint16_t a, uint16_t b);
uint16_t find_step(uint16_t V, unsigned int* seed);
uint32_t get_E(uint16_t V, uint8_t per_E);
void progress_bar(uint64_t a, uint64_t b, uint8_t* progres, uint8_t scale);

struct graph* get_random_data(uint16_t V, uint8_t per_E, uint8_t connected){

    if(per_E > 100){
        printf("ERROR: Percentage greater than 100 (?)\n");
        return NULL;
    }

    uint32_t E = get_E(V, per_E);
    printf("Number of edges: %u\n", E);

    if(E > 50000000){
        printf("ERROR: Too many edges\n");
        return NULL;
    }

    if(connected && E < V){
        printf("ERROR: Not enought edges\n");
        return NULL;
    }

    if(V==1){
        printf("ERROR: Too few vertexes\n");
        return NULL;
    }

    struct graph* g = malloc(sizeof(struct graph));

    if(g == NULL){

        printf("Error: Memory allocation failed for graph\n");
        return NULL;

    }

    g->v = V;
    g->e = E;

    g->adj_matrix = malloc((V - 1) * sizeof(uint16_t*));

    if(g->adj_matrix == NULL){

        printf("Error: Memory allocation failed for adj_matrix rows\n");
        free(g);
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
            return NULL;

        }

    }

    unsigned int seed;
    FILE* f = fopen("/dev/urandom", "rb");
    if(f && fread(&seed, sizeof(seed), 1, f) == 1){

        fclose(f);

    } else{

        if(f)
            fclose(f);
        
        seed = (unsigned int)time(NULL);

    }

    uint8_t progres = 0xff;

    if(per_E > 66){

        for(uint16_t i = 1; i < V; i++){

            progress_bar(i, V-1, &progres, per_E);

            for(uint16_t j = 0; j < i; j++)
                write_edge_weight(i, j, (rand_r(&seed) % EDGE_MAX_WEIGHT) + 1, g->adj_matrix);

        }

        uint32_t all_edges = ((uint64_t)V * ((uint64_t)V - 1)) / 2;    
        uint32_t edges_to_delete = all_edges - E;

        while(edges_to_delete > 0){

            uint16_t u = (rand_r(&seed) % V);
            uint16_t v = (rand_r(&seed) % V);
            
            if(u == v || get_edge_weight(u, v, g->adj_matrix) == 0)
                continue;
            
            write_edge_weight(u, v, 0, g->adj_matrix);
            edges_to_delete--;

            progress_bar(all_edges - edges_to_delete, all_edges, &progres, 100);

        }

        return g;

    }

    uint32_t edges_added = 0;

    if(connected){

        uint16_t step = find_step(V, &seed);
        uint16_t vs = 0, vsn = step;

        for(uint16_t i = 0; i < V - 1; i++){

            vs %= V;
            vsn %= V;
            
            write_edge_weight(vs, vsn, (rand_r(&seed) % EDGE_MAX_WEIGHT) + 1, g->adj_matrix);        //zakres wag: 1 - UINT16_MAX-1

            vs += step;
            vsn += step;

            progress_bar(i, E, &progres, 100);

        }

        edges_added = V - 1;

    }

    while(edges_added < E){

        uint16_t u = (rand_r(&seed) % V);
        uint16_t v = (rand_r(&seed) % V);
        
        if(u == v || get_edge_weight(u, v, g->adj_matrix) != 0)
            continue;
        
        write_edge_weight(u, v, (rand_r(&seed) % EDGE_MAX_WEIGHT) + 1, g->adj_matrix);
        edges_added++;

        progress_bar(edges_added, E, &progres, 100);

    }

    return g;

}

uint16_t gcd(uint16_t a, uint16_t b){

    uint16_t temp;

    while (b != 0){
        temp = a % b;

        a = b;
        b = temp;

    }

    return a;

}

uint16_t find_step(uint16_t V,  unsigned int* seed){

    uint16_t r;

    do{
        r = rand_r(seed) % (V - 1) + 1;
    } while(gcd(r, V) != 1);
        
    return r;

}

uint32_t get_E(uint16_t V, uint8_t per_E){

    return (uint32_t)(((uint64_t)V * ((uint64_t)V - 1) * per_E) / 200);

}

void progress_bar(uint64_t a, uint64_t b, uint8_t* progres, uint8_t scale){

    if((a*scale / b) == *progres)
        return;

    *progres = (a*scale / b);

    const int bar_width = 50;
    int pos = bar_width * *progres / 100;
    
    printf("\rData creation progress: [");
    for(int i = 0; i < bar_width; ++i){

        if(i < pos)
            printf("#");
        else if(i == pos)
            printf(">");
        else
            printf("-");
    }

    printf("] %d%%", *progres);
    fflush(stdout);

}
