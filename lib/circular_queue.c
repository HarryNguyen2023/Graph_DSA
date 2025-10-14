#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "circular_queue.h"

CQueueNode* cqueue_node_create (void *data)
{
  CQueueNode *node = (CQueueNode *)malloc(sizeof(CQueueNode));
  if (! node)
  {
    printf ("[%s,%d] Fail to allocate memory for cqueue node!\n",
            __func__, __LINE__);
    return NULL;
  }

  node->data  = data;
  node->next  = NULL;
  node->prev  = NULL;
}

CQueue* cqueue_create (int capacity) 
{
  CQueue *queue = (CQueue *)malloc(sizeof(CQueue));
  if (! queue)
  {
    printf("Error: Could not allocate memory for circular queue\n");
    return NULL;
  }
  queue->size = 0;
  queue->capacity = capacity;
  queue->front = NULL;
  queue->rear = NULL;

  return queue;
}

int cqueue_delete (CQueue* queue) 
{
  if (queue == NULL)
  {
    printf("[%s,%d] Error: Circular Queue is NULL\n",
            __func__, __LINE__);
    return -1;
  }

  CQueueNode* current = queue->front;
  CQueueNode* next = NULL;
  int count = 0;
  while (current != NULL && count < queue->size)
  {
    next = current->next;
    free(current);
    current = next;
    count++;
  }
  free(queue);
  queue = NULL;
  return 0;
}

int cqueue_is_empty (CQueue* queue) 
{
  if (queue == NULL)
  {
    printf("[%s,%d] Error: Circular Queue is NULL\n",
            __func__, __LINE__);
    return -1;
  }
  return queue->size == 0;
}

int cqueue_is_full (CQueue* queue) 
{
  if (queue == NULL)
  {
    printf("[%s,%d] Error: Circular Queue is NULL\n",
            __func__, __LINE__);
    return -1;
  }
  return queue->size == queue->capacity;
}

int cqueue_enqueue (CQueue* queue, void* data)
{
  CQueueNode *new_node = NULL;

  if (queue == NULL)
  {
    printf("[%s,%d] Error: Circular Queue is NULL\n",
            __func__, __LINE__);
    return -1;
  }

  if (cqueue_is_full (queue))
  {
    printf("[%s,%d] Error: Circular Queue is full\n",
            __func__, __LINE__);
    return -1;
  }

  new_node = cqueue_node_create (data);
  if (! new_node)
    return -1;
  
  if (queue->rear == NULL)
  {
    queue->rear   = new_node;
    queue->front  = new_node;
  }
  else
  {
    queue->rear->next = new_node;
    new_node->prev    = queue->rear;
    queue->rear       = new_node;
  }

  queue->size++;
  return 0;
}

int cqueue_dequeue (CQueue* queue, void **data)
{
  CQueueNode *temp = NULL;

  if (queue == NULL)
  {
    printf("[%s,%d] Error: Circular Queue is NULL\n",
            __func__, __LINE__);
    return -1;
  }

  if (cqueue_is_empty (queue))
  {
    printf("[%s,%d] Error: Circular Queue is empty\n",
            __func__, __LINE__);
    return -1;
  }

  temp = queue->front;
  *data = temp->data;
  queue->front = queue->front->next;
  if (queue->front)
    queue->front->prev = NULL;
  else
    queue->rear = NULL;

  free (temp);
  queue->size--;
  return 0;
}

void cqueue_dump (CQueue* queue, int(*cqueue_data_dump)(void *))
{
  void *data = NULL;

  if (queue == NULL)
  {
    printf("[%s,%d] Error: Circular Queue is NULL\n",
            __func__, __LINE__);
    return;
  }

  if (cqueue_is_empty (queue))
  {
    printf("[%s,%d] Error: Circular Queue is empty\n",
            __func__, __LINE__);
    return;
  }

  while (! cqueue_is_empty(queue))
  {
    cqueue_dequeue (queue, &data);
    if (data
        && cqueue_data_dump)
      cqueue_data_dump (data);
  }
}