#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "graph.h"
#include "stack.h"
#include "queue.h"
#include "priority_queue.h"

#define MAX_VERTICES 100
#define INFINITY     10000
#define UNKNOW_VETEX -1

Vertex *vertex_init (int id, int dest, int weight)
{
  Vertex* newVertex = (Vertex*)malloc(sizeof(Vertex));
  newVertex->id           = id;
  newVertex->edge.dest    = dest;
  newVertex->edge.weight  = weight;
  newVertex->next         = NULL;
  newVertex->prev         = NULL;
  return newVertex;
}

Graph* graph_init(int numVertices) 
{
  if (numVertices <= 0 || numVertices > MAX_VERTICES)
  {
    printf("Error: Number of vertices must be between 1 and %d\n", MAX_VERTICES);
    return NULL;
  }

  Graph* graph = (Graph*)malloc(sizeof(Graph));
  if (! graph)
  {
	  printf ("Fail to allocate memory for graph!\n");
	  return NULL;
  }

  graph->numVertices = numVertices;
  graph->vertices = malloc(numVertices * sizeof(Vertex *));
  if (! graph->vertices)
  {
	  printf ("Fail to allocate memory for vertices!\n");
	  return NULL;
  }

  for (int i = 0; i < numVertices; i++)
    graph->vertices[i] = NULL;

  return graph;
}

int graph_add_edge(Graph* graph, int src, int dest, int weight)
{
  Vertex* newVertex = NULL;

  if (src >= graph->numVertices 
      || dest >= graph->numVertices
      || src < 0
      || dest < 0
      || src == dest) 
  {
    printf("Error: Vertex index out of bounds\n");
    return -1;
  }

  /* Add edge for src->dest */
  newVertex = vertex_init(src, dest, weight);
  if (newVertex == NULL)
  {
    printf("Error: Could not allocate memory for new vertex\n");
    return -1;
  }

  if (graph->vertices[src] == NULL)
    graph->vertices[src] = newVertex;
  else
  {
    Vertex* temp = graph->vertices[src];
    while (temp->next != NULL) {
      temp = temp->next;
    }
    temp->next = newVertex;
    newVertex->prev = temp;
  }

  /* Add edge for dest->src */
  newVertex = vertex_init(dest, src, weight);
  if (newVertex == NULL) {
    printf("Error: Could not allocate memory for new vertex\n");
    return -1;
  }

  if (graph->vertices[dest] == NULL)
    graph->vertices[dest] = newVertex;
  else
  {
    Vertex* temp = graph->vertices[dest];
    while (temp->next != NULL) {
      temp = temp->next;
    }
    temp->next = newVertex;
    newVertex->prev = temp;
  }
}

int graph_remove_edge(Graph* graph, int src, int dest) 
{
  if (src >= graph->numVertices 
      || dest >= graph->numVertices
      || src < 0
      || dest < 0
      || src == dest) 
  {
    printf("Error: Vertex index out of bounds\n");
    return -1;
  }

  /* Remove edge for src->dest */
  Vertex* temp = graph->vertices[src];
  while (temp != NULL
        && temp->id != src
        && temp->edge.dest != dest) {
    temp = temp->next;
  }
  if (temp == NULL) {
    printf("Error: Edge does not exist\n");
    return -1;
  }

  if (temp->prev != NULL)
    temp->prev->next = temp->next;
  else
    graph->vertices[src] = temp->next;

  if (temp->next != NULL)
    temp->next->prev = temp->prev;

  free(temp);
  temp = NULL;

  /* Remove edge for dest->src */
  temp = graph->vertices[dest];
  while (temp != NULL
        && temp->id != dest
        && temp->edge.dest != src) {
    temp = temp->next;
  }
  if (temp == NULL) {
    printf("Error: Edge does not exist\n");
    return -1;
  }

  if (temp->prev != NULL)
    temp->prev->next = temp->next;
  else
    graph->vertices[dest] = temp->next;

  if (temp->next != NULL)
    temp->next->prev = temp->prev;

  free(temp);
  temp = NULL;
}

void graph_print(Graph* graph) 
{
  if (graph == NULL) {
    printf("[%s,%d] Error: Graph is NULL\n", __func__, __LINE__);
    return;
  }

  for (int i = 0; i < graph->numVertices; i++) {
    Vertex* temp = graph->vertices[i];
    printf("Vertex %d:", temp->id);
    while (temp != NULL) {
      printf(" -> (%d:%d)", temp->edge.dest, temp->edge.weight);
      temp = temp->next;
    }
    printf("\n");
  }
}

void graph_deinit (Graph* graph) 
{
  if (graph == NULL)
  {
    printf("[%s,%d] Error: Graph is NULL\n", __func__, __LINE__);
    return;
  }

  for (int i = 0; i < graph->numVertices; i++) {
    Vertex* current = graph->vertices[i];
    Vertex* next = NULL;
    while (current != NULL) {
      next = current->next;
      free(current);
      current = next;
    }
  }
  free(graph->vertices);
  free(graph);
  graph = NULL;
}

int graph_DFS (Graph* graph, int start_vertex)
{
  int *visited_vertices = NULL;
  Stack *stack = NULL;
  int* top;

  if (! graph)
  {
    printf ("[%s,%d] Error, Graph is NULL\n", __func__, __LINE__);
    return -1;
  }

  visited_vertices = (int *)calloc(graph->numVertices, sizeof(int));
  if (! visited_vertices)
  {
    printf ("[%s,%d] Error: Fail to allocate memory for visited array\n", __func__, __LINE__);
    return -1;
  }
  memset((void *)visited_vertices, 0, graph->numVertices * sizeof(int));

  stack = stack_create ();
  if (! stack)
  {
    printf ("[%s,%d] Error: Fail to allocate memory for stack\n", __func__, __LINE__);
    return -1;
  }

  /* Push the first vertex into the stack */
  stack_push (stack, &start_vertex);
  visited_vertices[start_vertex] = 1;
  printf ("Visited ");

  while (! stack_is_empty (stack))
  {
    /* Pop the top of the stack */
    top = (int *)stack_pop (stack);
    if (top)
    {
      printf (" -> %d", *top);
      Vertex *temp = graph->vertices[*top];
      /* Push unvisited adjacencies into the stack */
      while (temp)
      {
        if (visited_vertices[temp->edge.dest] == 0)
        {
          visited_vertices[temp->edge.dest] = 1;
          stack_push (stack, &temp->edge.dest);
        }
        temp = temp->next;
      }
    }
  }
  printf ("\n");

  /* Clean up */
  stack_delete (stack);
  return 0;
}

int graph_BFS (Graph* graph, int start_vertex)
{
  int *visited_vertices = NULL;
  Queue *queue = NULL;
  int* front;

  if (! graph)
  {
    printf ("[%s,%d] Error, Graph is NULL\n", __func__, __LINE__);
    return -1;
  }

  visited_vertices = (int *)calloc(graph->numVertices, sizeof(int));
  if (! visited_vertices)
  {
    printf ("[%s,%d] Error: Fail to allocate memory for visited array\n", __func__, __LINE__);
    return -1;
  }

  queue = queue_create ();
  if (! queue)
  {
    printf ("[%s,%d] Error: Fail to allocate memory for queue\n", __func__, __LINE__);
    return -1;
  }

  /* Push the first vertex into the stack */
  queue_enqueue (queue, &start_vertex);
  visited_vertices[start_vertex] = 1;
  printf ("Visited ");

  while (! queue_is_empty (queue))
  {
    /* Pop the top of the stack */
    queue_dequeue (queue, (void **)&front);
    if (front)
    {
      printf (" -> %d", *front);
      Vertex *temp = graph->vertices[*front];
      /* Push unvisited adjacencies into the stack */
      while (temp)
      {
        if (visited_vertices[temp->edge.dest] == 0)
        {
          visited_vertices[temp->edge.dest] = 1;
          queue_enqueue (queue, &temp->edge.dest);
        }
        temp = temp->next;
      }
    }
  }
  printf ("\n");

  /* Clean up */
  queue_delete (queue);
  return 0;
}

int graph_path_node_cmp (void *p1, void *p2)
{
  PathNode *path1 = (PathNode *)p1;
  PathNode *path2 = (PathNode *)p2;

  if (!path1 && !path2)
    return 0;

  if (!path1 || !path2)
    return -1;

  if (*(path1->dist) > *(path2->dist))
    return 1;
  else if (*(path1->dist) < *(path2->dist))
    return -1;
  else
  {
    if (path1->V->edge.weight > path2->V->edge.weight)
      return 1;
    else if (path1->V->edge.weight < path2->V->edge.weight)
      return -1;
    else
    {
      if (path1->V->edge.dest > path2->V->edge.dest)
        return 1;
      else if (path1->V->edge.dest < path2->V->edge.dest)
        return -1;
      else
        return 0;
    }
  }

  return 0;
}

int graph_path_node_dump (void *p)
{
  PathNode *path = (PathNode *)p;
  if (! path)
    return -1;

  printf ("(s:%d,d:%d,w:%d)", path->V->id, path->V->edge.dest, path->V->edge.weight);
}

int graph_print_prev_node (int *prev_node, int src, int dest, int size)
{
  int prev;
  Stack stack = {0};

  if (!prev_node || !size || dest >= size)
    return -1;

  prev = prev_node[dest];
  while (prev != src && prev < size)
  {
    if (prev != UNKNOW_VETEX)
      stack_push (&stack, (void *)&prev);
    prev = prev_node[prev];
  }

  printf ("Path from %d to %d: ", src, dest);
  while (! stack_is_empty(&stack))
  {
    int* temp = stack_pop (&stack);
    if (temp)
    {
      printf ("%d ", *temp);
    }
  }
  printf("\n");

  return 0;
}

int dijkstra (Graph *graph, int src, int *distance, int *prev_node,
              int (*pq_cmp)(void *, void *), int (*pq_node_dump)(void *))
{
  int i, temp_dist;
  PriorityQueue *pq = NULL;
  Vertex *min = NULL, *temp;
  PathNode *path = NULL;

  if (!graph || !graph->vertices || !graph->numVertices
      || !distance || !prev_node)
    return -1;

  if (src >= graph->numVertices)
  {
    printf ("Error: src % larger then number of vertices %d\n",
            src, graph->numVertices);
    return -1;
  }

  pq = pq_create (graph->numVertices, pq_cmp, pq_node_dump);
  if (! pq)
  {
    printf ("[%s,%d] Fail to create priority queue!\n",
            __func__, __LINE__);
  }

  path = (PathNode *)calloc(graph->numVertices, sizeof(PathNode));
  if (! path)
  {
    printf ("[%s,%d] Error: Can not allocate memory for path!\n", __func__, __LINE__);
    return -1;
  }

  for (i = 0; i < graph->numVertices; ++i)
  {
    if (i != src)
    {
      distance[i]     = INFINITY;
      prev_node[i]    = UNKNOW_VETEX;
    }
    else
    {
      distance[i]     = 0;
      prev_node[i]    = src;
    }
    path[i].dist = &distance[i];
    path[i].V    = graph->vertices[i];
    pq_add (pq, &path[i]);
  }

  while (! pq_is_empty (pq))
  {
    min = ((PathNode *)pq_extract_top(pq))->V;
    while (min)
    {
      temp_dist = distance[min->id] + min->edge.weight;
      if (temp_dist < distance[min->edge.dest])
      {
        distance[min->edge.dest]  = temp_dist;
        prev_node[min->edge.dest] = min->id;
      }
      min = min->next;
    }
  }

  pq_deinit (pq);

  return 0;
}

int graph_dijkstra (Graph *graph, int src)
{
  int *distance, *prev_node, rv, i;

  if (!graph || !graph->vertices || !graph->numVertices)
    return -1;

  distance = (int *)malloc(graph->numVertices * sizeof (int));
  if (!distance)
  {
    printf ("[%s,%d] Fail to allocated memory for distance array!\n",
            __func__, __LINE__);
  }

  prev_node = (int *)malloc(graph->numVertices * sizeof (int));
  if (!prev_node)
  {
    printf ("[%s,%d] Fail to allocated memory for distance array!\n",
            __func__, __LINE__);
  }

  rv = dijkstra (graph, src, distance, prev_node, graph_path_node_cmp, graph_path_node_dump);
  if (rv != 0)
    return rv;

  for (i = 0; i < graph->numVertices; i++)
  {
    if (i != src)
    {
      printf ("Distance from %d to %d: %d\n", src, i, distance[i]);
      graph_print_prev_node (prev_node, src, i, graph->numVertices);
    }
  }

  free (distance);
  free (prev_node);

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
  
  graph = graph_init(numVertices);
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
  graph_DFS (graph, 3);

  graph_dijkstra (graph, 3);

  return 0;
}