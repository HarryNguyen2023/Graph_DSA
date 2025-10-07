#ifndef __QUEUE_H__
#define __QUEUE_H__

typedef struct QueueNode
{
  void* data;
  struct QueueNode* next;
  struct QueueNode* prev;
} QueueNode;

typedef struct Queue
{
  int size;
  QueueNode* front;
  QueueNode* rear;
} Queue;

Queue* queue_create (void);
int queue_delete (Queue* queue);
int queue_is_empty (Queue* queue);
int queue_enqueue (Queue* queue, void* data);
int queue_dequeue (Queue* queue, void **data);

#endif /* __QUEUE_H__ */