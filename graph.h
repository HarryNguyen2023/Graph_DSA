#ifndef __GRAPH_H__
#define __GRAPH_H__

typedef struct Edge
{
  int dest;
  int weight;
} Edge;

typedef struct Vertex
{
  Edge edge;
  struct Vertex* next;
  struct Vertex* prev;
} Vertex;

typedef struct Graph
{
  int numVertices;
  Vertex** vertices;
} Graph;

#endif /* __GRAPH_H__ */