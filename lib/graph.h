#ifndef __GRAPH_H__
#define __GRAPH_H__

typedef struct Edge
{
  int dest;
  int weight;
} Edge;

typedef struct Vertex
{
  int id;
  Edge edge;
  struct Vertex* next;
  struct Vertex* prev;
} Vertex;

typedef struct PathNode
{
  Vertex* V;
  int* dist;
} PathNode;

typedef struct Graph
{
  int numVertices;
  Vertex** vertices;
} Graph;

Graph* graph_init(int numVertices);
void graph_deinit (Graph* graph);

int graph_add_edge(Graph* graph, int src, int dest, int weight);
int graph_remove_edge(Graph* graph, int src, int dest);
void graph_print(Graph* graph);

int graph_DFS (Graph* graph, int start_vertex);
int graph_BFS (Graph* graph, int start_vertex);
int graph_dijkstra (Graph *graph, int src);
int graph_bellman_ford (Graph *graph, int src);
int graph_kruskal (Graph *graph);
int graph_prim (Graph *graph, int start);

#endif /* __GRAPH_H__ */