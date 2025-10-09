#include"libs.h"
#include"get_data.h"
#include"dikstra.h"

int main(int argc, char** argv){

    struct graph* data = NULL;

    switch (argc){
    case 2:
        data = get_data_from_file(argv[1]);
        break;
    
    case 3:
        data = get_random_data(atoi(argv[1]), atoi(argv[2]));
        break;

    default:
        printf("Incorrect args were passed.\n");
        return 1;
    }
        

    
    free(data->adj_matrix);
    free(data);
    data = NULL;

    return 0;
    
}
