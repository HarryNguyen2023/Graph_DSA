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

#define MIN(A,B)    (((A) < (B)) ? (A) : (B))

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

  stack = stack_create (graph->numVertices);
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

static void stack_dump_int (void *data)
{
  int *num = NULL;

  if (! data)
    return;

  num = (int *)data;
  printf ("-> %d ", *num);
  return;
}

int graph_print_path_recursive (Graph *graph, int *prev_node, int i_src, int i_dest)
{
  if (! graph || ! prev_node)
    return -1;

  if (prev_node[i_dest] == i_src)
    return i_dest;

  if (graph->vertices[i_dest])
    printf ("-> %d ", graph->vertices[graph_print_path_recursive (graph, prev_node, i_src, prev_node[i_dest])]->id);
}

int graph_print_path (Graph *graph, int *prev_node, int src, int dest)
{
  int prev, i_dest, i_src, rv, *temp = NULL, i;

  if (!graph || !prev_node)
    return -1;

  i_dest = graph_get_vertex_by_id (graph, dest);
  if (i_dest == UNKNOW_VETEX)
  {
    printf ("[%s,%d] There no vertex %d in the graph!\n", __func__, __LINE__, dest);
    return 0;
  }

  i_src = graph_get_vertex_by_id (graph, src);
  if (i_src == UNKNOW_VETEX)
  {
    printf ("[%s,%d] There no vertex %d in the graph!\n", __func__, __LINE__, src);
    return 0;
  }

  printf ("Path from %d to %d: %d ", src, dest, src);
  graph_print_path_recursive (graph, prev_node, i_src, i_dest);
  printf ("-> %d\n", dest);

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
        prev_node[i_dest] = i;
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
    if (graph->vertices[i]
        && graph->vertices[i]->id != src)
    {
      printf ("\nDistance from %d to %d: %d\n", src, graph->vertices[i]->id, distance[i]);
      graph_print_path (graph, prev_node, src, graph->vertices[i]->id);
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
    i_dest = graph_get_vertex_by_id (graph, temp->edge.dest);
    if (i_dest != UNKNOW_VETEX)
    {
      distance[i_dest]   = temp->edge.weight;
      prev_node[i_dest]  = i;
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
        prev_node[i_dest] = i;
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
    if (graph->vertices[i]
        && graph->vertices[i]->id != src)
    {
      printf ("\nDistance from %d to %d: %d\n", src, graph->vertices[i]->id, distance[i]);
      graph_print_path (graph, prev_node, src, graph->vertices[i]->id);
    }
  }

  free (distance);
  free (prev_node);

  return 0;
}

int graph_edge_cmp (void *v1, void *v2)
{
  Vertex *vertex_1 = (Vertex *)v1;
  Vertex *vertex_2 = (Vertex *)v2;

  if (!vertex_1 && !vertex_2)
    return 0;

  if (!vertex_1 || !vertex_2)
    return 1;

  if ((vertex_1->edge.weight) > (vertex_2->edge.weight))
    return 1;
  else if ((vertex_1->edge.weight) < (vertex_2->edge.weight))
    return -1;
  else
  {
    if ((vertex_1->edge.dest) > (vertex_2->edge.dest))
      return 1;
    else if ((vertex_1->edge.dest) < (vertex_2->edge.dest))
      return -1;
    else
    {
      if (vertex_1->id > vertex_2->id)
        return 1;
      else if (vertex_1->id < vertex_2->id)
        return -1;
      else
        return 0;
    }
  }

  return 0;
}

int kruskal (Graph *graph, Graph* minimum_span_tree)
{
  int *visited = NULL;
  int graph_max_edges = 0, i, rv = 0, i_src, i_dest;
  PriorityQueue *pq = NULL;
  Vertex *temp = NULL;

  if ((! graph) || (! minimum_span_tree)
      || (! graph->numVertices) || (! graph->vertices)
      || (! minimum_span_tree->numVertices) || (! minimum_span_tree->vertices))
    return -1;

  visited = (int *)malloc(graph->numVertices * sizeof (int));
  if (! visited)
  {
    printf ("[%s,%d] Fail to allocate memory for visited array\n", __func__, __LINE__);
    return -1;
  }
  memset (visited, 0, graph->numVertices * sizeof (int));

  graph_max_edges = ((graph->numVertices) * (graph->numVertices - 1));
  pq = pq_create (graph_max_edges, graph_edge_cmp, NULL);
  if (! pq)
  {
    printf ("[%s,%d] Fail to create priority queue!\n", __func__, __LINE__);
    return -1;
  }

  /* Add all edges into the priority queue */
  for (i = 0; i < graph->numVertices; ++i)
  {
    if (graph->vertices[i])
    {
      temp = graph->vertices[i];
      while (temp)
      {
        pq_add (pq, (void *)temp);

        temp = temp->next;
      }
    }
  }

  temp = NULL;
  while (! pq_is_empty (pq))
  {
    temp = (Vertex *)pq_extract_top(pq);
    if (temp)
    {
      i_src   = graph_get_vertex_by_id (graph, temp->id);
      i_dest  = graph_get_vertex_by_id (graph, temp->edge.dest);
      if (i_src != UNKNOW_VETEX
          && i_dest != UNKNOW_VETEX
          && ! (visited[i_src] == 1
                && visited[i_dest] == 1))
      {
        graph_add_edge (minimum_span_tree, temp->id, temp->edge.dest, temp->edge.weight);

        visited[i_src]  = 1;
        visited[i_dest] = 1;
      }
    }
  }

  pq_deinit (pq);
  if (visited)
    free (visited);
  visited = NULL;
  return 0;

ERR_EXIT:
  pq_deinit (pq);
  if (visited)
    free (visited);
  visited = NULL;
  return -1;
}

int graph_kruskal (Graph *graph)
{
  Graph *minimum_span_tree = NULL;
  int rv = 0;

  if (! graph)
    return -1;

  minimum_span_tree = graph_init (graph->numVertices);
  if (! minimum_span_tree)
  {
    printf ("[%s,%d] Fail to create minimum spanning tree!\n", __func__, __LINE__);
    goto EXIT;
  }
  rv = kruskal (graph, minimum_span_tree);
  if (rv != 0)
  {
    printf ("[%s,%d] Fail to perform kruskal algorithm\n", __func__, __LINE__);
    goto EXIT;
  }

  printf ("Minimum Spanning Tree:\n");
  graph_print (minimum_span_tree);

EXIT:
  if (minimum_span_tree)
    free (minimum_span_tree);
  minimum_span_tree = NULL;
  return rv;
}

int prim (Graph *graph, Graph* minimum_span_tree, int start)
{
  int *visited = NULL;
  int graph_max_edges = 0, i, rv = 0, i_src, i_dest, i_start;
  PriorityQueue *pq = NULL;
  Vertex *temp = NULL, *top = NULL;

  if ((! graph) || (! minimum_span_tree)
      || (! graph->numVertices) || (! graph->vertices)
      || (! minimum_span_tree->numVertices) || (! minimum_span_tree->vertices))
    return -1;

  i_start = graph_get_vertex_by_id (graph, start);
  if (i_start == UNKNOW_VETEX)
  {
    printf ("[%s,%d] Start vertex %d is not in the graph\n", __func__, __LINE__, start);
    return -1;
  }

  visited = (int *)malloc(graph->numVertices * sizeof (int));
  if (! visited)
  {
    printf ("[%s,%d] Fail to allocate memory for visited array\n", __func__, __LINE__);
    return -1;
  }
  memset (visited, 0, graph->numVertices * sizeof (int));

  graph_max_edges = graph->numVertices - 1;
  pq = pq_create (graph_max_edges, graph_edge_cmp, NULL);
  if (! pq)
  {
    printf ("[%s,%d] Fail to create priority queue!\n", __func__, __LINE__);
    goto ERR_EXIT;
  }

  while (i_start != UNKNOW_VETEX && visited[i_start] == 0 && graph->vertices[i_start])
  {
    visited[i_start] = 1;
    temp = graph->vertices[i_start];
    while (temp)
    {
      pq_add (pq, (void *)temp);

      temp = temp->next;
    }

    do
    {
      top = (Vertex *) pq_extract_top (pq);
      if (top)
      {
        i_start = graph_get_vertex_by_id (graph, top->edge.dest);
        if (i_start != UNKNOW_VETEX
            && visited[i_start] == 0)
        {
          graph_add_edge (minimum_span_tree, top->id, top->edge.dest, top->edge.weight);
          break;
        }
      }
    } while (! pq_is_empty (pq));

    pq_flush (pq);
  }

  pq_deinit (pq);
  if (visited)
    free (visited);
  visited = NULL;
  return 0;

ERR_EXIT:
  if (visited)
    free (visited);
  visited = NULL;
  return -1;
}

int graph_prim (Graph *graph, int start)
{
  Graph *minimum_span_tree = NULL;
  int rv = 0;

  if (! graph)
    return -1;

  minimum_span_tree = graph_init (graph->numVertices);
  if (! minimum_span_tree)
  {
    printf ("[%s,%d] Fail to create minimum spanning tree!\n", __func__, __LINE__);
    goto EXIT;
  }

  rv = prim (graph, minimum_span_tree, start);
  if (rv != 0)
  {
    printf ("[%s,%d] Fail to perform kruskal algorithm\n", __func__, __LINE__);
    goto EXIT;
  }

  printf ("Minimum Spanning Tree:\n");
  graph_print (minimum_span_tree);

EXIT:
  if (minimum_span_tree)
    free (minimum_span_tree);
  minimum_span_tree = NULL;
  return rv;
}

int bfs_util (Graph *graph, int s, int t, int *parent)
{
  int *visited = NULL, *front = NULL, i_s, i_t, i_src, i_dest;
  Queue *queue = NULL;
  Vertex *temp = NULL;

  if (! graph || ! graph->numVertices || ! graph->vertices
      || ! parent)
    return 0;

  visited = (int *)malloc(graph->numVertices * sizeof(int));
  if (! visited)
  {
    printf ("[%s,%d] Fail to allocated memory for visisted array\n", __func__, __LINE__);
    return 0;
  }
  memset (visited, 0, graph->numVertices * sizeof(int));

  queue = queue_create ();
  if (! queue)
  {
    printf ("[%s,%d] Fail to create queue for BFS algorithm\n", __func__, __LINE__);
    goto ERR_EXIT;
  }

  i_s = graph_get_vertex_by_id (graph, s);
  if (i_s == UNKNOW_VETEX)
  {
    printf ("[%s,%d] Source vertex %d is not in the graph\n", __func__, __LINE__, s);
    goto ERR_EXIT;
  }

  i_t = graph_get_vertex_by_id (graph, t);
  if (i_t == UNKNOW_VETEX)
  {
    printf ("[%s,%d] Sink vertex %d is not in the graph\n", __func__, __LINE__, t);
    goto ERR_EXIT;
  }

  queue_enqueue (queue, (void *)&s);
  visited[i_s] = 1;

  while (! queue_is_empty (queue))
  {
    queue_dequeue (queue, (void **)&front);
    i_src = graph_get_vertex_by_id (graph, *front);
    if (i_src == UNKNOW_VETEX)
    {
      printf ("[%s,%d] Vertex %d is not in the graph\n", __func__, __LINE__, *front);
      goto ERR_EXIT2;
    }

    temp = graph->vertices[i_src];
    while (temp)
    {
      i_dest = graph_get_vertex_by_id (graph, temp->edge.dest);
      if (i_dest == UNKNOW_VETEX)
      {
        printf ("[%s,%d] Vertex %d is not in the graph\n", __func__, __LINE__, temp->edge.dest);
        goto ERR_EXIT2;
      }

      if (! visited[i_dest] && temp->edge.weight > 0)
      {
        visited[i_dest] = 1;
        parent[i_dest] = i_src;
        if (i_dest == i_t)
          goto SUC_EXIT;

        queue_enqueue (queue, (void *)&temp->edge.dest);
      }

      temp = temp->next;
    }
  }

ERR_EXIT2:
  queue_delete (queue);
ERR_EXIT:
  if (visited)
    free (visited);
  visited = NULL;
  return 0;

SUC_EXIT:
  queue_delete (queue);
  if (visited)
    free (visited);
  visited = NULL;
  return 1;
}

void graph_copy (Graph *dest, Graph *src)
{
  int i = 0;
  Vertex *temp;

  if (!src || !src->numVertices || !src->vertices
      || !dest)
    return;

  for (i = 0; i < src->numVertices; ++i)
  {
    if (src->vertices[i])
    {
      temp = src->vertices[i];
      while (temp)
      {
        graph_add_edge (dest, temp->id, temp->edge.dest, temp->edge.weight);

        temp = temp->next;
      }
    }
  }
}

void graph_add_weight (Graph *graph, int src, int dest, int weight)
{
  int i = 0;
  Vertex *temp = NULL;

  if (! graph || ! graph->numVertices || ! graph->vertices)
    return;

  i = graph_get_vertex_by_id (graph, src);
  if (i == UNKNOW_VETEX)
  {
    printf ("[%s,%d] Vertex %d is not in the graph\n", __func__, __LINE__, src);
    return;
  }

  if (graph->vertices[i])
  {
    temp = graph->vertices[i];
    while (temp)
    {
      if (temp->edge.dest == dest)
        break;
      temp = temp->next;
    }

    temp->edge.weight += weight;
  }

  i = graph_get_vertex_by_id (graph, dest);
  if (i == UNKNOW_VETEX)
  {
    printf ("[%s,%d] Vertex %d is not in the graph\n", __func__, __LINE__, dest);
    return;
  }

  if (graph->vertices[i])
  {
    temp = graph->vertices[i];
    while (temp)
    {
      if (temp->edge.dest == src)
        break;
      temp = temp->next;
    }

    temp->edge.weight -= weight;
  }
}

int graph_ford_fulkerson (Graph *graph, int s, int t)
{
  Graph *rgraph = NULL;
  Vertex *temp;
  int i_s, i_t, i, i_parent, *parent = NULL, max_flow = 0, path_flow, path_capacity = 0;

  if (! graph || ! graph->numVertices || ! graph->vertices)
    return -1;

  i_s = graph_get_vertex_by_id (graph, s);
  if (i_s == UNKNOW_VETEX)
  {
    printf ("[%s,%d] Source vertex %d is not in the graph\n", __func__, __LINE__, s);
    return -1;
  }

  i_t = graph_get_vertex_by_id (graph, t);
  if (i_t == UNKNOW_VETEX)
  {
    printf ("[%s,%d] Sink vertex %d is not in the graph\n", __func__, __LINE__, t);
    return -1;
  }

  rgraph = graph_init (graph->numVertices);
  if (! rgraph)
  {
    printf ("[%s,%d] Fail to create the residual graph\n", __func__, __LINE__);
    return -1;
  }
  graph_copy (rgraph, graph);

  parent = (int *)malloc(graph->numVertices * sizeof (int));
  if (! parent)
  {
    printf ("[%s,%d] Fail to allocate memory for parent arry\n", __func__, __LINE__);
    goto ERR_EXIT;
  }
  for (int i = 0; i < graph->numVertices; ++i)
    parent[i] = UNKNOW_VETEX;

  while (bfs_util (rgraph, s, t, parent))
  {
    path_flow = INFINITY;
    for (i = i_t; i != i_s; i = parent[i])
    {
      i_parent = parent[i];
      if (i_parent == UNKNOW_VETEX)
        break;

      if (rgraph->vertices[i_parent] && rgraph->vertices[i])
      {
        temp = rgraph->vertices[i_parent];
        while (temp)
        {
          if (temp->edge.dest == rgraph->vertices[i]->id)
            break;

          temp = temp->next;
        }
        if (temp)
          path_capacity = temp->edge.weight;
      }
      path_flow = MIN(path_flow, path_capacity);
    }

    for (i = i_t; i != i_s; i = parent[i])
    {
      i_parent = parent[i];
      if (i_parent == UNKNOW_VETEX)
        break;

      if (rgraph->vertices[i_parent] && rgraph->vertices[i])
      {
        graph_add_weight (rgraph, rgraph->vertices[i_parent]->id, rgraph->vertices[i]->id, path_flow);
      }
    }

    max_flow += path_flow;
  }

  printf ("Max Flow of the network is: %d\n", max_flow);

  if (parent)
    free (parent);
  parent = NULL;

  graph_deinit (rgraph);
  return max_flow;

ERR_EXIT:
  graph_deinit (rgraph);
  return -1;
}

GraphMat *graph_matrix_init (int numVertices)
{
	int i = 0;
  GraphMat *g = NULL;

	if (! numVertices)
		return NULL;

  g = (GraphMat *)malloc(sizeof (GraphMat));
  if (! g)
    return NULL;

  g->numVertices = numVertices;
	g->vertices = (int **)calloc(numVertices, sizeof (int *));
	if (! g->vertices)
		return NULL;

	for (i = 0; i < numVertices; ++i)
	{
		*(g->vertices + i) = calloc (numVertices, sizeof(int));
		if (! *(g->vertices + i))
			goto ERR;
	}
	return g;

ERR:
	for (i; i >= 0; i--)
	{
		free (*(g->vertices + i));
	}
	free (g->vertices);
  return NULL;
}

void graph_matrix_deinit (GraphMat *g)
{
  int i = 0;

  if (! g)
    return;

  if (g->vertices)
  {
    for (i = 0; i < g->numVertices; ++i)
      free (*(g->vertices + i));

    free (g->vertices);
  }
  g->vertices = NULL;
  g->numVertices = 0;
  free (g);
  g = NULL;
}

int graph_convert_list_to_matrix (Graph *graph, GraphMat **g_mat)
{
  int i = 0, j = 0;
  Vertex *temp = NULL;

  if (! graph || ! graph->numVertices || ! graph->vertices)
    return -1;

  *g_mat = graph_matrix_init (graph->numVertices);
  if (! g_mat)
  {
    printf ("[%s,%d] Fail to create graph matrix\n", __func__, __LINE__);
    return -1;
  }

  for (i = 0; i < (*g_mat)->numVertices; ++i)
    for (j = 0; j < (*g_mat)->numVertices; ++j)
      (*g_mat)->vertices[i][j] = INFINITY;

  for (i = 0; i < graph->numVertices; ++i)
  {
    if (graph->vertices[i])
    {
      temp = graph->vertices[i];
      while (temp)
      {
        (*g_mat)->vertices[temp->id][temp->edge.dest] = temp->edge.weight;

        temp = temp->next;
      }
    }
  }

  return 0;
}

int floyd_warshall (Graph *graph, GraphMat **g_mat)
{
  int i, j, k;

  if (! graph || ! graph->numVertices || ! graph->vertices)
    return -1;

  graph_convert_list_to_matrix (graph, g_mat);
  if (! (*g_mat))
  {
    printf ("[%s,%d] Fail to create graph matrix\n", __func__, __LINE__);
    return -1;
  }

  for (k = 0; k < (*g_mat)->numVertices; ++k)
  {
    for (i = 0; i < (*g_mat)->numVertices; ++i)
    {
      for (j = 0; j < (*g_mat)->numVertices; ++j)
      {
        if ((*g_mat)->vertices[i][k] + (*g_mat)->vertices[k][j] < (*g_mat)->vertices[i][j])
          (*g_mat)->vertices[i][j] = (*g_mat)->vertices[i][k] + (*g_mat)->vertices[k][j];
      }
    }
  }

  return 0;
}

int graph_floyd_warshall (Graph *graph)
{
  GraphMat *g_mat = NULL;
  int rv = 0, i, j;

  if (! graph || ! graph->numVertices || ! graph->vertices)
    return -1;

  rv = floyd_warshall (graph, &g_mat);
  if (rv != 0)
  {
    printf ("[%s,%d] Fail to perform the floyd warshall algorithm\n", __func__, __LINE__);
    return rv;
  }

  if (g_mat)
  {
    printf ("The shortest path matrix: \n");
    for (i = 0; i < g_mat->numVertices; ++i)
    {
      for (j = 0; j < g_mat->numVertices; ++j)
        if (g_mat->vertices[i][j] == INFINITY)
          printf ("%4s ", "INF");
        else
          printf ("%4d ", g_mat->vertices[i][j]);
      printf ("\n");
    }
    printf ("\n");
  }

  if (g_mat)
    graph_matrix_deinit (g_mat);

  return 0;
}