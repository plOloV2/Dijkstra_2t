#include"que.h"

struct priority_queue* pq_create(uint32_t capacity){

    struct priority_queue* pq = malloc(sizeof(struct priority_queue));
    if(!pq){
        printf("ERROR: Creating priority queue failed\n");
        return NULL;
    }

    pq->heap = calloc(capacity, sizeof(struct node));
    if(!pq->heap){
        printf("ERROR: Creating priority queue heap failed\n");
        free(pq);
        return NULL;
    }

    pq->capacity = capacity;
    pq->size = 0;

    return pq;

}

void pq_free(struct priority_queue* pq){

    free(pq->heap);
    free(pq);

}

void node_swap(struct node* a, struct node* b){

    struct node temp = *a;
    *a = *b;
    *b = temp;

}

void heapify_up(struct priority_queue* pq){

    uint32_t i = pq->size - 1;

    while(i > 0){

        uint32_t parent = (i - 1) / 2;

        if(pq->heap[parent].distance <= pq->heap[i].distance)
            break;

        node_swap(&pq->heap[parent], &pq->heap[i]);
        i = parent;

    }
    
}

void heapify_down(struct priority_queue* pq){

    uint32_t i = 0;

    while(1){

        uint32_t left = 2 * i + 1;
        uint32_t right = 2 * i + 2;
        uint32_t smallest = i;

        if(left < pq->size && pq->heap[left].distance < pq->heap[smallest].distance)
            smallest = left;

        if(right < pq->size && pq->heap[right].distance < pq->heap[smallest].distance)
            smallest = right;

        if(smallest == i)
            break;

        node_swap(&pq->heap[i], &pq->heap[smallest]);
        i = smallest;

    }
    
}

void pq_push(uint16_t vertex, uint32_t distance, struct priority_queue* pq){

    if(pq->size > pq->capacity){
        printf("ERROR: Heap not big enought\n");
        return;
    }

    pq->heap[pq->size].distance = distance;
    pq->heap[pq->size].v = vertex;

    pq->size++;

    heapify_up(pq);

}

uint8_t pq_pop(uint16_t* vertex, uint32_t* distance, struct priority_queue* pq){

    if(pq->size == 0){

        printf("ERROR: Cannot pop from empty heap\n");
        return 0;

    }

    pq->size--;

    node_swap(&pq->heap[0], &pq->heap[pq->size]);

    *vertex = pq->heap[pq->size].v;
    *distance = pq->heap[pq->size].distance;

    pq->heap[pq->size].distance = 0;
    pq->heap[pq->size].v = 0;

    heapify_down(pq);

    return 1;

}

uint8_t pq_empty(struct priority_queue* pq){

    return pq->size == 0;

}

uint32_t pq_peek_top_distance(struct priority_queue* pq){

    if(pq->size == 0)
        return UINT32_MAX;

    return pq->heap[0].distance;

}
