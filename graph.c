#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "graph.h"
#include "stack.h"
#include "queue.h"

#define MAX_VERTICES 100

Vertex *vertex_init (int id) 
{
  Vertex* newVertex = (Vertex*)malloc(sizeof(Vertex));
  newVertex->id = id;
  newVertex->next = NULL;
  newVertex->prev = NULL;
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

int graph_add_edge(Graph* graph, int src, int dest) 
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
  newVertex = vertex_init(dest);
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
  newVertex = vertex_init(src);
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
  while (temp != NULL && temp->id != dest) {
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
  while (temp != NULL && temp->id != src) {
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
    printf("Vertex %d:", i);
    Vertex* temp = graph->vertices[i];
    while (temp != NULL) {
      printf(" -> %d", temp->id);
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
        if (visited_vertices[temp->id] == 0)
        {
          visited_vertices[temp->id] = 1;
          stack_push (stack, &temp->id);
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
        if (visited_vertices[temp->id] == 0)
        {
          visited_vertices[temp->id] = 1;
          queue_enqueue (queue, &temp->id);
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

int main (int argc, char** argv) 
{
  Graph* graph = NULL;
  int numVertices = 0;

  printf ("Enter the number of vertices: ");
  scanf ("%d", &numVertices);
  
  graph = graph_init(numVertices);
  if (graph == NULL) {
    printf("Error: Could not initialize graph\n");
    return -1;
  }
  graph_add_edge(graph, 0, 1);
  graph_add_edge(graph, 0, 4);
  graph_add_edge(graph, 1, 2);
  graph_add_edge(graph, 1, 3);
  graph_add_edge(graph, 1, 4);
  graph_add_edge(graph, 2, 3);
  graph_add_edge(graph, 3, 4);

  graph_print(graph);
  graph_DFS (graph, 3);

  graph_remove_edge(graph, 1, 4);
  printf("After removing edge 1-4:\n");
  // graph_print(graph);
  graph_BFS (graph, 2);

  return 0;
}