#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "lib/graph.h"
#include "lib/stack.h"
#include "lib/queue.h"
#include "lib/priority_queue.h"
#include "lib/circular_buff.h"

#define CIRBUFF_MAX_SIZE    (100)
#define CIRBUFF_ELEM_SIZE   (4)

void cirbuff_print_int (void *data)
{
  int *num = NULL;

  if (!data)
    return;

  num = (int *)data;
  printf ("%d ", *num);
  return;
}

int cirbuff_test (void)
{
  CirBuffer *cir_buff = NULL;
  int i, write, read, num;
  
  cir_buff = cirbuff_create (CIRBUFF_MAX_SIZE, CIRBUFF_ELEM_SIZE);
  if (!cir_buff)
  {
    printf ("[%s,%d] Fail to create circular buffer\n", __func__, __LINE__);
    return -1;
  }

  for (i = 0; i < (CIRBUFF_MAX_SIZE / CIRBUFF_ELEM_SIZE); ++i)
  {
    write = cirbuff_write (cir_buff, (unsigned char *)&i, 1);
    if (write != 1)
    {
      printf ("[%s,%d] Fail to write %d to circular buffer\n",
              __func__, __LINE__, i);
      break;
    }
  }

  cirbuff_dump (cir_buff, cirbuff_print_int);

  printf ("Read circular buffer: ");
  for (i = 0; i < (CIRBUFF_MAX_SIZE / CIRBUFF_ELEM_SIZE) - 2; ++i)
  {
    read = cirbuff_read (cir_buff, (unsigned char *)&num, 1);
    if (read != 1)
    {
      printf ("[%s,%d] Fail to read %d to circular buffer\n",
              __func__, __LINE__, i);
      break;
    }

    printf ("%d ", num);
  }
  printf ("\n");

  cirbuff_dump (cir_buff, cirbuff_print_int);
  return 0;
}

int main (int argc, char** argv) 
{
  Graph* graph = NULL;
  PriorityQueue *pq = NULL;
  int numVertices = 0;
  Edge *edge = NULL;

  printf ("Enter the number of vertices: ");
  scanf ("%d", &numVertices);
  
  graph = graph_init (numVertices);
  if (graph == NULL) {
    printf("Error: Could not initialize graph\n");
    return -1;
  }
  graph_add_edge(graph, 0, 1, 5);
  graph_add_edge(graph, 0, 4, 7);
  graph_add_edge(graph, 1, 2, 1);
  graph_add_edge(graph, 1, 3, 4);
  graph_add_edge(graph, 1, 4, 1);
  graph_add_edge(graph, 2, 3, 4);
  graph_add_edge(graph, 3, 4, 3);

  graph_print(graph);
  graph_BFS (graph, 3);

  printf ("\nDijkstra's Algorithm: \n");
  graph_dijkstra (graph, 2);

  printf ("\nBellman-Ford Algorithm: \n");
  graph_bellman_ford (graph, 2);

  graph_deinit (graph);

  cirbuff_test ();
  return 0;
}