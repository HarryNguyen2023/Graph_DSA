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
#include "lib/binary_tree.h"
#include "lib/huffman_code.h"
#include "lib/hash_table.h"

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
  int numVertices = 6;
  Edge *edge = NULL;
  
  graph = graph_init (numVertices);
  if (graph == NULL) {
    printf("Error: Could not initialize graph\n");
    return -1;
  }
  graph_add_edge(graph, 0, 1, 8);
  graph_add_edge(graph, 0, 4, 3);
  graph_add_edge(graph, 1, 2, 9);
  graph_add_edge(graph, 2, 3, 2);
  graph_add_edge(graph, 4, 3, 7);
  graph_add_edge(graph, 4, 5, 4);
  graph_add_edge(graph, 5, 3, 5);

  graph_DFS (graph, 3);
  graph_print(graph);
  graph_BFS (graph, 3);

  printf ("\n************** Dijkstra's Algorithm *****************\n");
  graph_dijkstra (graph, 4);

  printf ("\n************** Bellman-Ford Algorithm **************** \n");
  graph_bellman_ford (graph, 3);

  printf ("\n************** Circular Buffer Test ***************** \n");
  cirbuff_test ();

  printf ("\n*************** Kruskal's Algorithm ***************** \n");
  graph_kruskal (graph);

  printf ("\n***************** Prim's Algorithm ****************** \n");
  graph_prim (graph, 1);

  printf ("\n************ Ford-Fulkerson's Algorithm ************* \n");
  /* Only use for directed graph */
  // graph_ford_fulkerson (graph, 0, 3);

  printf ("\n************ Floyd-Warshall's Algorithm ************* \n");
  graph_floyd_warshall (graph);

  graph_deinit (graph);

  printf ("\n************ Huffman Coding's Algorithm ************* \n");
  char str[] = "BCAADDDCCACACAC", *encoded_str = NULL, *arr = NULL, *decoded_str = NULL;
  int s_len = sizeof (str) / sizeof (char) - 1, encoded_str_len = 0, decoded_str_len = 0;
  BinTreeNode *root = NULL;
  HuffmanData *huffman_data = NULL;

  huffman_encoding (str, s_len, &encoded_str, &encoded_str_len, &root, &huffman_data);

  arr = (char *)calloc (11, sizeof(char));
  if (! arr) return -1;

  memset (arr, 0, 11 * sizeof (char));
  printf ("Output Huffman encoded string (len %d): %s\n", encoded_str_len, encoded_str);
  printf ("Output Huffman encoded tree:\n");
  huffman_code_tree_print (root, arr, 0);

  huffman_decoding (root, encoded_str, encoded_str_len, &decoded_str, &decoded_str_len);
  printf ("Output Huffman decoded string (len %d): %s\n", decoded_str_len, decoded_str);

  if (huffman_data) free(huffman_data);
  huffman_data = NULL;
  if (root) bin_tree_delete_util (root);
  root = NULL;
  if (encoded_str)  free(encoded_str);
  if (decoded_str)  free(decoded_str);

  return 0;
}