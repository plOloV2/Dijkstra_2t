#include"graph.h"

uint16_t get_edge_weight(uint16_t v, uint16_t u, uint16_t** adj_matrix){

    if(u == v)
        return 0;

    if(u < v){

        return adj_matrix[v - 1][u];

    } else{

        return adj_matrix[u - 1][v];

    }

}

void write_edge_weight(uint16_t v, uint16_t u, uint16_t weight, uint16_t** adj_matrix){

    if(u < v){

        adj_matrix[v - 1][u] = weight;

    } else{

        adj_matrix[u - 1][v] = weight;

    }

}
