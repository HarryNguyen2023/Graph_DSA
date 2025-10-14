#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "lib/graph.h"
#include "lib/stack.h"
#include "lib/queue.h"
#include "lib/priority_queue.h"

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
  return 0;
}