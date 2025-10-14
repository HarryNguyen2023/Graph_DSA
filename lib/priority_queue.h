#ifndef __PRIORITY_QUEUE_H__
#define __PRIORITY_QUEUE_H__

typedef struct
{
    void *data;
} PQNode;

typedef struct
{
    PQNode* nodes;
    int capacity;
    int size;

    /* Util function */
    int (*pq_cmp)(void *, void *);
    int (*pq_node_dump)(void *);
} PriorityQueue;

PriorityQueue* pq_create(int capacity,
                         int (*pq_cmp)(void *, void *),
                         int (*pq_node_dump)(void *));
int pq_deinit(PriorityQueue* pq);
int pq_add (PriorityQueue *pq, void *data);
int pq_delete (PriorityQueue *pq, void *data);
void pq_print (PriorityQueue *pq);
PQNode* pq_peek_top (PriorityQueue *pq);
void* pq_extract_top (PriorityQueue *pq);
int pq_is_empty (PriorityQueue *pq);

#endif /* __PRIORITY_QUEUE_H__ */