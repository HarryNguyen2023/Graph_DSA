#ifndef __CIRCULAR_QUEUE_H__
#define __CIRCULAR_QUEUE_H__

typedef struct CQueueNode
{
  void* data;
  struct CQueueNode* next;
  struct CQueueNode* prev;
} CQueueNode;

typedef struct Queue
{
  int size;
  int capacity;
  CQueueNode* front;
  CQueueNode* rear;
} CQueue;

CQueue* cqueue_create (int capacity);
int cqueue_delete (CQueue* queue);
int cqueue_is_empty (CQueue* queue);
int cqueue_enqueue (CQueue* queue, void* data);
int cqueue_dequeue (CQueue* queue, void **data);

#endif /* __CIRCULAR_QUEUE_H__ */