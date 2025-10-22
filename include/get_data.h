#ifndef GET_DAT
#define GET_DAT

#include"libs.h"
#include"graph.h"

struct graph* get_data_from_file(char* file_name);

struct graph* get_random_data(uint16_t V, uint8_t per_E, uint8_t connected, uint8_t prog_bar);

#endif
