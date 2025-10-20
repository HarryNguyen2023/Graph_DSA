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

int graph_get_vertex_by_id (Graph* graph, int id)
{
  int i = 0;

  if (! graph)
    return UNKNOW_VETEX;

  for (i = 0; i < graph->numVertices; i++)
  {
    if (graph->vertices[i]
        && graph->vertices[i]->id == id)
      break;
  }

  if (i == graph->numVertices)
    return UNKNOW_VETEX;

  return i;
}

int graph_get_empty_vertex (Graph* graph)
{
  int i = 0;

  if (! graph)
    return UNKNOW_VETEX;

  for (i = 0; i < graph->numVertices; i++)
  {
    if (graph->vertices[i] == NULL)
      break;
  }

  if (i == graph->numVertices)
  {
    printf ("[%s,%d] Graph is already full\n", __func__, __LINE__);
    return UNKNOW_VETEX;
  }

  return i;
}

int graph_add_edge(Graph* graph, int src, int dest, int weight)
{
  Vertex* newVertex = NULL;
  int i = 0;

  if (src < 0
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

  /* Get location for vertex */
  i = graph_get_vertex_by_id (graph, src);
  if (i == UNKNOW_VETEX)
  {
    i = graph_get_empty_vertex (graph);
    if (i == UNKNOW_VETEX)
      return -1;
  }

  if (graph->vertices[i] == NULL)
    graph->vertices[i] = newVertex;
  else
  {
    Vertex* temp = graph->vertices[i];
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

  /* Get location for vertex */
  i = graph_get_vertex_by_id (graph, dest);
  if (i == UNKNOW_VETEX)
  {
    i = graph_get_empty_vertex (graph);
    if (i == UNKNOW_VETEX)
      return -1;
  }

  if (graph->vertices[i] == NULL)
    graph->vertices[i] = newVertex;
  else
  {
    Vertex* temp = graph->vertices[i];
    while (temp->next != NULL) {
      temp = temp->next;
    }
    temp->next = newVertex;
    newVertex->prev = temp;
  }
}

int graph_remove_edge(Graph* graph, int src, int dest) 
{
  int i = 0;

  if (src < 0
      || dest < 0
      || src == dest) 
  {
    printf("Error: Vertex index out of bounds\n");
    return -1;
  }

  /* Remove edge for src->dest */
  i = graph_get_vertex_by_id (graph, src);
  if (i == UNKNOW_VETEX)
  {
    printf ("[%s,%d] Error: There is no edge with src %d in graph\n",
           __func__, __LINE__, src);
    return -1;
  }

  Vertex* temp = graph->vertices[i];
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
    graph->vertices[i] = temp->next;

  if (temp->next != NULL)
    temp->next->prev = temp->prev;

  free(temp);
  temp = NULL;

  /* Remove edge for dest->src */
  i = graph_get_vertex_by_id (graph, dest);
  if (i == UNKNOW_VETEX)
  {
    printf ("[%s,%d] Error: There is no edge with src %d in graph\n",
           __func__, __LINE__, dest);
    return -1;
  }

  temp = graph->vertices[i];
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
    graph->vertices[i] = temp->next;

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
    if (temp)
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
      if (current)
        free(current);
      current = next;
    }
  }

  if (graph->vertices)
    free(graph->vertices);
  graph->vertices = NULL;

  free(graph);
  graph = NULL;
}

int graph_DFS (Graph* graph, int start_vertex)
{
  int *visited_vertices = NULL;
  Stack *stack = NULL;
  int *top, i;

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
      i = graph_get_vertex_by_id (graph, *top);
      if (i == UNKNOW_VETEX)
        goto EXIT;

      Vertex *temp = graph->vertices[i];
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

EXIT:
  printf ("\n");

  /* Clean up */
  stack_delete (stack);
  free (visited_vertices);
  return 0;
}

int graph_BFS (Graph* graph, int start_vertex)
{
  int *visited_vertices = NULL;
  Queue *queue = NULL;
  int *front, i;

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
      i = graph_get_vertex_by_id (graph, *front);
      if (i == UNKNOW_VETEX)
        goto EXIT;

      Vertex *temp = graph->vertices[i];
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

EXIT:
  printf ("\n");

  /* Clean up */
  queue_delete (queue);
  free (visited_vertices);
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

int graph_print_prev_node (Graph *graph, int *prev_node, int src, int dest, int size)
{
  int prev, i;
  Stack stack = {0};

  if (!prev_node || !size || dest >= size)
    return -1;

  i = graph_get_vertex_by_id (graph, dest);
  if (i == UNKNOW_VETEX)
    return 0;

  prev = prev_node[i];
  while (prev != src && prev < size)
  {
    if (prev != UNKNOW_VETEX)
      stack_push (&stack, (void *)&prev);

    i = graph_get_vertex_by_id (graph, prev);
    if (i == UNKNOW_VETEX)
      break;
    prev = prev_node[i];
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
  int i, i_dest, temp_dist;
  PriorityQueue *pq = NULL;
  Vertex *min = NULL, *temp;
  PathNode *path = NULL;

  if (!graph || !graph->vertices || !graph->numVertices
      || !distance || !prev_node)
    return -1;

  pq = pq_create (graph->numVertices, pq_cmp, pq_node_dump);
  if (! pq)
  {
    printf ("[%s,%d] Fail to create priority queue!\n",
            __func__, __LINE__);
    return -1;
  }

  path = (PathNode *)calloc(graph->numVertices, sizeof(PathNode));
  if (! path)
  {
    printf ("[%s,%d] Error: Can not allocate memory for path!\n", __func__, __LINE__);
    return -1;
  }

  for (i = 0; i < graph->numVertices; ++i)
  {
    if (graph->vertices[i]
        && graph->vertices[i]->id != src)
      distance[i]     = INFINITY;
    else
      distance[i]     = 0;

    prev_node[i]    = UNKNOW_VETEX;
  }

  /* Update distance for src's neighbors */
  i = graph_get_vertex_by_id (graph, src);
  if (i == UNKNOW_VETEX)
  {
    printf ("[%s,%d] Error: There is no edge with src %d in graph\n",
           __func__, __LINE__, src);
    goto EXIT;
  }

  temp = graph->vertices[i];
  while (temp)
  {
    i = graph_get_vertex_by_id (graph, temp->edge.dest);
    if (i != UNKNOW_VETEX)
    {
      distance[i]   = temp->edge.weight;
      prev_node[i]  = graph_get_vertex_by_id (graph, src);
    }

    temp = temp->next;
  }

  for (i = 0; i < graph->numVertices; ++i)
  {
    if (graph->vertices[i]
        && graph->vertices[i]->id != src)
    {
      path[i].dist    = &distance[i];
      path[i].V       = graph->vertices[i];
      pq_add (pq, &path[i]);
    }
  }

  while (! pq_is_empty (pq))
  {
    /* Get the vertex with closest distance to src */
    min = ((PathNode *)pq_extract_top(pq))->V;
    while (min)
    {
      i = graph_get_vertex_by_id (graph, min->id);
      if (i == UNKNOW_VETEX)
        continue;

      i_dest = graph_get_vertex_by_id (graph, min->edge.dest);
      if (i_dest == UNKNOW_VETEX)
        continue;

      temp_dist = distance[i] + min->edge.weight;
      if (temp_dist < distance[i_dest])
      {
        distance[i_dest]  = temp_dist;
        prev_node[i_dest] = min->id;
      }
      min = min->next;
    }
  }

EXIT:
  free (path);
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
    return -1;
  }

  prev_node = (int *)malloc(graph->numVertices * sizeof (int));
  if (!prev_node)
  {
    printf ("[%s,%d] Fail to allocated memory for distance array!\n",
            __func__, __LINE__);
    return -1;
  }

  rv = dijkstra (graph, src, distance, prev_node, graph_path_node_cmp, graph_path_node_dump);
  if (rv != 0)
    return rv;

  for (i = 0; i < graph->numVertices; i++)
  {
    if (i != src
        && graph->vertices[i])
    {
      printf ("Distance from %d to %d: %d\n", src, graph->vertices[i]->id, distance[i]);
      // graph_print_prev_node (graph, prev_node, src, i, graph->numVertices);
    }
  }

  free (distance);
  free (prev_node);

  return 0;
}

int bellman_ford (Graph *graph, int src, int *distance, int *prev_node)
{
  int i, i_dest, temp_dist;
  Vertex *temp = NULL;

  if (!graph || !graph->vertices || !graph->numVertices
      || !distance || !prev_node)
    return -1;

  if (src >= graph->numVertices)
  {
    printf ("Error: src % larger then number of vertices %d\n",
            src, graph->numVertices);
    return -1;
  }

  for (i = 0; i < graph->numVertices; ++i)
  {
    if (graph->vertices[i]
        && graph->vertices[i]->id != src)
      distance[i]     = INFINITY;
    else
      distance[i]     = 0;

    prev_node[i]  = UNKNOW_VETEX;
  }

  /* Update distance for src's neighbors */
  i = graph_get_vertex_by_id (graph, src);
  if (i == UNKNOW_VETEX)
  {
    printf ("[%s,%d] Error: There is no edge with src %d in graph\n",
           __func__, __LINE__, src);
    return -1;
  }

  temp = graph->vertices[i];
  while (temp)
  {
    i = graph_get_vertex_by_id (graph, temp->edge.dest);
    if (i != UNKNOW_VETEX)
    {
      distance[i]   = temp->edge.weight;
      prev_node[i]  = src;
    }

    temp = temp->next;
  }

  for (i = 0; i < graph->numVertices; ++i)
  {
    temp = graph->vertices[i];
    while (temp)
    {
      i = graph_get_vertex_by_id (graph, temp->id);
      if (i == UNKNOW_VETEX)
        continue;

      i_dest = graph_get_vertex_by_id (graph, temp->edge.dest);
      if (i_dest == UNKNOW_VETEX)
        continue;

      temp_dist = distance[i] + temp->edge.weight;
      if (temp_dist < distance[i_dest])
      {
        distance[i_dest]  = temp_dist;
        prev_node[i_dest] = temp->id;
      }
      temp = temp->next;
    }
  }

  /* Detect negative weight cycle */
  for (i = 0; i < graph->numVertices; ++i)
  {
    temp = graph->vertices[i];
    while (temp)
    {
      i = graph_get_vertex_by_id (graph, temp->id);
      if (i == UNKNOW_VETEX)
        continue;

      i_dest = graph_get_vertex_by_id (graph, temp->edge.dest);
      if (i_dest == UNKNOW_VETEX)
        continue;

      temp_dist = distance[i] + temp->edge.weight;
      if (temp_dist < distance[i_dest])
      {
        printf ("[%s,%d] Error: Detected negative weight cycles!\n", __func__, __LINE__);
        return -1;
      }
      temp = temp->next;
    }
  }

  return 0;
}

int graph_bellman_ford (Graph *graph, int src)
{
  int *distance, *prev_node, rv, i;

  if (!graph || !graph->vertices || !graph->numVertices)
    return -1;

  distance = (int *)malloc(graph->numVertices * sizeof (int));
  if (!distance)
  {
    printf ("[%s,%d] Fail to allocated memory for distance array!\n",
            __func__, __LINE__);
    return -1;
  }

  prev_node = (int *)malloc(graph->numVertices * sizeof (int));
  if (!prev_node)
  {
    printf ("[%s,%d] Fail to allocated memory for distance array!\n",
            __func__, __LINE__);
    return -1;
  }

  rv = bellman_ford (graph, src, distance, prev_node);
  if (rv != 0)
    return rv;

  for (i = 0; i < graph->numVertices; i++)
  {
    if (i != src)
    {
      printf ("Distance from %d to %d: %d\n", src, i, distance[i]);
      // graph_print_prev_node (graph, prev_node, src, i, graph->numVertices);
    }
  }

  free (distance);
  free (prev_node);

  return 0;
}