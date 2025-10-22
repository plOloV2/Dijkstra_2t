#include"libs.h"

uint32_t** alloc_uint32_array(uint16_t size, char* error_mesage_array_name){

    uint32_t** array = malloc(sizeof(uint32_t*)*2);
    if(!array){

        printf("ERROR: Dijkstra bidirect 1th %s** malloc failed\n", error_mesage_array_name);
        return NULL;

    }

    array[0] = malloc(size * sizeof(uint32_t));
    array[1] = malloc(size * sizeof(uint32_t));
    if(!array[0] || !array[1]){

        printf("ERROR: Dijkstra bidirect 1th %s* malloc failed\n", error_mesage_array_name);
        free(array);
        return NULL;

    }

    memset(array[0], 0xff, size * sizeof(uint32_t));
    memset(array[1], 0xff, size * sizeof(uint32_t));

    return array;

}

void free_uint32_array(uint32_t** array){

    free(array[0]);
    free(array[1]);
    free(array);
    array = NULL;

}

uint8_t compare_uint32_arrays(uint32_t** array_A, uint32_t** array_B, uint16_t size){

    for(uint16_t i = 0; i < size; i++)
        if(array_A[0][i] != array_B[0][i] || array_A[1][i] != array_B[1][i])
            return 0;

    return 1;

}
