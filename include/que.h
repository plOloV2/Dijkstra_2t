#ifndef QUE
#define QUE

#include"libs.h"

struct node{
    uint16_t v;
    uint32_t distance;
};

struct priority_queue {
    struct node* heap;
    uint32_t size;
    uint32_t capacity;
};

#endif