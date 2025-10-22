#ifndef PRIORITY_QUE
#define PRIORITY_QUE

#include"libs.h"
#include"que.h"

struct priority_queue* pq_create(uint32_t capacity);

void pq_free(struct priority_queue* pq);

void pq_push(uint16_t vertex, uint32_t distance, struct priority_queue* pq);

uint8_t pq_pop(uint16_t* vertex, uint32_t* distance, struct priority_queue* pq);

uint8_t pq_empty(struct priority_queue* pq);

uint32_t pq_peek_top_distance(struct priority_queue* pq);

#endif
