#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "priority_queue.h"

PriorityQueue* pq_create(int capacity,
                         int (*pq_cmp)(void *, void *),
                         int (*pq_node_dump)(void *))
{
  PriorityQueue* pq = (PriorityQueue*)malloc(sizeof(PriorityQueue));
  if (!pq)
    return NULL;

  pq->nodes = (PQNode*)malloc(capacity * sizeof(PQNode));
  if (! pq->nodes)
  {
    free(pq);
    return NULL;
  }

  pq->capacity      = capacity;
  pq->size          = 0;
  pq->pq_cmp        = pq_cmp;
  pq->pq_node_dump  = pq_node_dump;
  return pq;
}

int pq_deinit (PriorityQueue* pq)
{
  if (!pq)
    return -1;

  free(pq->nodes);
  free(pq);
  return 0;
}

void pq_node_swap (PQNode *a, PQNode *b)
{
  PQNode temp;

  if (!a || !b)
    return;

  memcpy (&temp, a, sizeof (PQNode));
  memcpy (a, b, sizeof (PQNode));
  memcpy (b, &temp, sizeof (PQNode));
}

/*
 * Utilize heap data structure for Priotiy Queue
 * If i is the index of the node, then
 *    2*i + 1  : is the left child of that node
 *    2*i + 2  : is the right child of that node
 *    i/2 - 1  : is the parent of that node
 */
int pq_heapify_util (PriorityQueue *pq, int i)
{
  int smallest, left_child, right_child;

  if (!pq || !pq->nodes || pq->size <= 1)
    return -1;

  smallest = i;
  left_child = 2*i + 1;
  right_child = 2*i + 2;

  if (left_child < pq->size && (pq->pq_cmp)
      && (*pq->pq_cmp)(pq->nodes[left_child].data, pq->nodes[smallest].data) < 0)
    smallest = left_child;
  if (right_child < pq->size && (pq->pq_cmp)
      && (*pq->pq_cmp)(pq->nodes[right_child].data, pq->nodes[smallest].data) < 0)
    smallest = right_child;

  if (smallest != i)
  {
    pq_node_swap (&pq->nodes[smallest], &pq->nodes[i]);
    pq_heapify_util (pq, smallest);
  }
}

int pq_heapify (PriorityQueue *pq)
{
  if (!pq || !pq->nodes || pq->size <= 1)
    return -1;

  return pq_heapify_util (pq, (pq->size/2 - 1));
}

PQNode *pq_find_node (PriorityQueue *pq, void *data)
{
  int i, found = false;

  if (!pq || !pq->nodes || !pq->size)
    return NULL;

  for (i = 0; i < pq->size; ++i)
  {
    if ((pq->pq_cmp)
        && (*pq->pq_cmp)(pq->nodes[i].data, data) == 0)
    {
      found = true;
      break;
    }
  }

  if (found)
    return &pq->nodes[i];
  else
    return NULL;
}

void pq_print (PriorityQueue *pq)
{
  int i;

  if (!pq || !pq->nodes || !pq->size)
    return;

  printf ("Priority Queue:\n");
  for (i = 0; i < pq->size; ++i)
  {
    if (pq->pq_node_dump)
      (*pq->pq_node_dump)(pq->nodes[i].data);
    printf ("\n");
  }
}

int pq_add_node (PriorityQueue *pq, void *data)
{
  if (!pq || !pq->nodes
      || pq->size == pq->capacity)
    return -1;

  pq->nodes[pq->size++].data = data;
  pq_heapify (pq);
}

int pq_add (PriorityQueue *pq, void *data)
{
  if (!pq || !pq->nodes || !data
      || pq->size == pq->capacity)
    return -1;

  if (pq_find_node (pq, data) != NULL)
    return 1;

  pq_add_node (pq, data);
}

int pq_delete_node (PriorityQueue *pq, PQNode *pq_node)
{
  if (!pq || !pq->nodes || !pq_node || !pq->size)
    return -1;

  /* Swap the delete node with the last node */
  if (&pq->nodes[pq->size - 1] != pq_node)
    pq_node_swap (pq_node, &pq->nodes[pq->size - 1]);

  pq->size--;
  for (int i = pq->size / 2 - 1; i >= 0; i--)
    pq_heapify_util (pq, i);

  return 0;
}

int pq_delete (PriorityQueue *pq, void *data)
{
  if (!pq || !pq->nodes || !data || !pq->size)
    return -1;

  PQNode *node = pq_find_node (pq, data);
  if (node == NULL)
    return -1;

  pq_delete_node (pq, node);
  return 0;
}

PQNode* pq_peek_top (PriorityQueue *pq)
{
  if (!pq || !pq->nodes|| !pq->size)
    return NULL;

  return &pq->nodes[0];
}

void* pq_extract_top (PriorityQueue *pq)
{
  int rv = 0;
  void *data = NULL;

  if (!pq || !pq->nodes || !pq->size)
    return NULL;

  data = (&(pq->nodes[0]))->data;
  rv = pq_delete (pq, data);
  if (rv != 0)
  {
    printf ("[%s,%d] Fail to delete the root node from PQ!", __func__, __LINE__);
    return NULL;
  }

  return data;
}

int pq_is_empty (PriorityQueue *pq)
{
  if (!pq || !pq->nodes)
    return -1;

  return (pq->size == 0);
}