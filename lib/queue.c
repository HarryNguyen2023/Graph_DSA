#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"

Queue* queue_create (int capacity)
{
  Queue *queue = (Queue *)malloc(sizeof(Queue));
  if (! queue)
  {
    printf("Error: Could not allocate memory for queue\n");
    return NULL;
  }

  queue->capacity = capacity;
  queue->size     = 0;
  queue->front    = NULL;
  queue->rear     = NULL;

  return queue;
}

int queue_delete (Queue* queue) 
{
  if (queue == NULL)
  {
    printf("[%s,%d] Error: Queue is NULL\n", __func__, __LINE__);
    return -1;
  }

  QueueNode* current = queue->front;
  QueueNode* next = NULL;
  while (current != NULL)
  {
    next = current->next;
    free(current);
    current = next;
  }
  free(queue);
  queue = NULL;
  return 0;
}

int queue_is_empty (Queue* queue) 
{
  if (queue == NULL)
  {
    printf("[%s,%d] Error: Queue is NULL\n", __func__, __LINE__);
    return -1;
  }
  return queue->size == 0;
}

int queue_enqueue (Queue* queue, void *data) 
{
  if (queue == NULL)
  {
    printf("[%s,%d] Error: Queue is NULL\n", __func__, __LINE__);
    return -1;
  }

  if (queue->size >= queue->capacity)
  {
    printf("[%s,%d] Error: Queue is already full\n", __func__, __LINE__);
    return -1;
  }

  QueueNode* newNode = (QueueNode*)malloc(sizeof(QueueNode));
  if (! newNode)
  {
    printf("Error: Could not allocate memory for new queue node\n");
    return -1;
  }
  newNode->data = data;
  newNode->next = NULL;
  newNode->prev = NULL;

  if (queue->rear == NULL)
  {
    queue->front = newNode;
    queue->rear = newNode;
  }
  else
  {
    queue->rear->next = newNode;
    newNode->prev = queue->rear;
    queue->rear = newNode;
  }
  queue->size++;
  return 0;
}

int queue_dequeue (Queue* queue, void **data) 
{
  if (queue == NULL)
  {
    printf("[%s,%d] Error: Queue is NULL\n", __func__, __LINE__);
    return -1;
  }
  if (queue->front == NULL)
  {
    // printf("Error: Queue is empty, cannot dequeue\n");
    return -1;
  }

  QueueNode* temp = queue->front;
  *data = temp->data;
  queue->front = queue->front->next;
  if (queue->front != NULL)
    queue->front->prev = NULL;
  else
    queue->rear = NULL;

  free(temp);
  queue->size--;
  return 0;
}

int queue_delete_node (Queue* queue, QueueNode *node)
{
  QueueNode *prev;

  if (queue == NULL || node == NULL)
  {
    printf("[%s,%d] Error: Queue is NULL\n", __func__, __LINE__);
    return -1;
  }

  if (node->prev)
    node->prev->next = node->next;

  if (node->next)
    node->next->prev = node->prev;

  if (node == queue->front)
    queue->front = node->next;

  if (node == queue->rear)
    queue->rear = node->prev;

  queue->size -= 1;
  free(node);
  return 0;
}