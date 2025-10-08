#ifndef GET_DAT
#define GET_DAT

#include"libs.h"
#include"graph.h"

struct graph* get_data_from_file(char* file_name, uint8_t file_name_lenght);

struct graph* get_random_data(uint8_t v, uint8_t e);

#endif
