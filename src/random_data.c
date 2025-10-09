#include"libs.h"
#include"graphs.h"
#include<time.h>

uint16_t gcd(uint16_t a, uint16_t b);
uint16_t find_step(uint16_t V, unsigned int* seed);

struct graph* get_random_data(uint16_t V, uint16_t E){

    if(E < V){
        printf("ERROR: Not enought edges\n");
        return NULL;
    }

    if(V==1){
        printf("ERROR: Too few vertexes\n");
        return NULL;
    }

    if(E > (V * (V - 1))/2){
        printf("ERROR: Too many edges\n");
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

    uint16_t step = find_step(V, &seed);
    uint16_t vs = 0, vsn = step;

    for(uint16_t i = 0; i < V - 1; i++){

        vs %= V;
        vsn %= V;
        
        write_edge_weight(vs, vsn, (rand_r(&seed) % 0xfffe) + 1, g->adj_matrix);

        vs += step;
        vsn += step;

    }

    uint16_t edges_added = V - 1;
    
    while(edges_added < E){

        uint16_t u = (rand_r(&seed) % V);
        uint16_t v = (rand_r(&seed) % V);
        
        if(u == v || get_edge_weight(u, v, g->adj_matrix) != 0)
            continue;
        
        write_edge_weight(u, v, (rand_r(&seed) % 0xfffe) + 1, g->adj_matrix);
        edges_added++;

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
