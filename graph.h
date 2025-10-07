#ifndef __GRAPH_H__
#define __GRAPH_H__

typedef struct Vertex
{
  int id;
  struct Vertex* next;
  struct Vertex* prev;
} Vertex;

typedef struct Graph
{
  int numVertices;
  Vertex** vertices;
} Graph;

#endif /* __GRAPH_H__ */