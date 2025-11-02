#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "lib/graph.h"
#include "lib/stack.h"
#include "lib/queue.h"
#include "lib/priority_queue.h"
#include "lib/circular_buff.h"
#include "lib/binary_tree.h"
#include "lib/huffman_code.h"
#include "lib/hash_table.h"
#include "lib/sort.h"

#define CIRBUFF_MAX_SIZE    (100)
#define CIRBUFF_ELEM_SIZE   (4)
#define MAX(A,B)            ((A) > (B)) ? A : B
#define MIN_RAND            1
#define MAX_RAND            100
#define SORT_ARR_LEN        500

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

int huffman_coding_test (void)
{
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

void measure_function_time_clock(void (*func)(void))
{
  clock_t start, end;
  double cpu_time_used;

  start = clock();
  func();
  end = clock();

  cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

  printf("CPU time used: %.6f seconds\n", cpu_time_used);
  
  return;
}

void reverse_string (char *str)
{
  char temp = '\0';
  int i, len = 0;

  if (! str)
    return;

  len = strlen(str);
  for (i = 0; i < len / 2; ++i)
  {
    temp = str[i];
    str[i] = str[len - i - 1];
    str[len - i - 1] = temp;
  }
}

void lcs_util (char *str_1, char *str_2, char** lcs_str, int *lcs_len)
{
  int str_len_1 = 0, str_len_2 = 0, i, j;
  unsigned char **str_mat = NULL;
  char *reverse_str = NULL;

  if (! str_1 || ! str_2)
    return;

  str_len_1 = strlen (str_1);
  str_len_2 = strlen (str_2);
  if (! str_len_1 || ! str_len_2)
  {
    printf ("[%s,%d] Error: Invalid string len str1:%d,str2:%d\n",
            __func__, __LINE__, str_len_1, str_len_2);
    return;
  }

  str_mat = (unsigned char **)calloc((str_len_1 + 1), sizeof(unsigned char *));
  if (! str_mat)
  {
    printf ("[%s,%d] Fail to allocate memory for the string matrix\n", __func__, __LINE__);
    return;
  }

  for (i = 0; i < (str_len_1 + 1); ++i)
  {
    *(str_mat + i) = (unsigned char *)calloc((str_len_2 + 1), sizeof(unsigned char));
    if (! *(str_mat + i))
    {
      printf ("[%s,%d] Fail to allocate memory for the string matrix column %d\n", __func__, __LINE__, i);
      goto EXIT;
    }
    memset (*(str_mat + i), 0, (str_len_2 + 1) * sizeof(unsigned char));
  }

  for (i = 1; i < str_len_1 + 1; i++)
  {
    for (j = 1; j < str_len_2 + 1; j++)
    {
      if (str_1[i - 1] == str_2[j - 1])
        str_mat[i][j] = str_mat[i - 1][j - 1] + 1;
      else
        str_mat[i][j] = MAX(str_mat[i - 1][j], str_mat[i][j - 1]);
    }
  }

  i = str_len_1;
  j = str_len_2;
  while (i && j)
  {
    if (str_mat[i][j] == str_mat[i - 1][j])
      i--;
    else if (str_mat[i][j] == str_mat[i][j - 1])
      j--;
    else
    {
      (*lcs_len)++;
      reverse_str = (char *)realloc(reverse_str, (*lcs_len + 1) * sizeof(char));
      if (! reverse_str)
        goto EXIT;

      reverse_str[*lcs_len - 1]  = str_1[i - 1];
      reverse_str[*lcs_len]      = '\0';
      i--;
      j--;
    }
  }

  *lcs_str = (char *)malloc((*lcs_len + 1) * sizeof(char));
  if (! *lcs_str)
    goto EXIT;

  strcpy (*lcs_str, reverse_str);
  reverse_string (*lcs_str);

EXIT:
  if (reverse_str)  free (reverse_str);
  reverse_str = NULL;
  for (i = i - 1; i >= 0; i--)
  {
    if (*(str_mat + i))
      free (*(str_mat + i));
    *(str_mat + i) = NULL;
  }
  free (str_mat);
  str_mat = NULL;
  return;
}

void lcs (char *str_1, char *str_2)
{
  char *lcs_str = NULL;
  int lcs_len = 0;

   if (! str_1 || ! str_2)
    return;

  lcs_util (str_1, str_2, &lcs_str, &lcs_len);

  if (lcs_str && lcs_len)
  {
    printf ("The LCS of string %s vs string %s is: %s, lenght %d\n",
            str_1, str_2, lcs_str, lcs_len);
  }

  if (lcs_str)  free (lcs_str);
  lcs_str = NULL;
  return;
}

int rand_int (int min, int max)
{
  if (min >= max)
  {
    printf ("[%s,%d] Invalid input min %d must smaller than max",
            __func__, __LINE__, min, max);
    return -1;
  }
  return (rand() % (max - min + 1)) + min;
}

void generate_rand_array (int arr[], int size)
{
  int i;

  if (! arr || ! size)
    return;

  for (i = 0; i < size; ++i)
    arr[i] = rand_int (MIN_RAND, MAX_RAND);
  return;
}

void bubble_sort_test (void)
{
  int array[SORT_ARR_LEN] = {0};
  int i, size;

  size = sizeof (array) / sizeof (int);
  generate_rand_array (array, size);

  bubble_sort (array, size);

  sort_array_dump (array, size, "Bubble Sort");
}

void selection_sort_test (void)
{
  int array[SORT_ARR_LEN] = {0};
  int i, size;

  size = sizeof (array) / sizeof (int);
  generate_rand_array (array, size);

  selection_sort (array, size);

  sort_array_dump (array, size, "Selection Sort");
}

void insertion_sort_test (void)
{
  int array[SORT_ARR_LEN] = {0};
  int i, size;

  size = sizeof (array) / sizeof (int);
  generate_rand_array (array, size);

  insertion_sort (array, size);

  sort_array_dump (array, size, "Insertion Sort");
}

void count_sort_test (void)
{
  int array[SORT_ARR_LEN] = {0};
  int i, size, *out_arr = NULL;

  size = sizeof (array) / sizeof (int);
  generate_rand_array (array, size);

  count_sort (array, size, &out_arr);

  if (out_arr)
    sort_array_dump (out_arr, size, "Count Sort");
}

void radix_sort_test (void)
{
  int array[SORT_ARR_LEN] = {0};
  int i, size;

  size = sizeof (array) / sizeof (int);
  generate_rand_array (array, size);

  radix_sort (array, size);

  sort_array_dump (array, size, "Radix Sort");
}

void bucket_sort_test (void)
{
  int array[SORT_ARR_LEN] = {0};
  int i, size;

  size = sizeof (array) / sizeof (int);
  generate_rand_array (array, size);

  bucket_sort (array, size);

  sort_array_dump (array, size, "Bucket Sort");
}

void heap_sort_test (void)
{
  int array[SORT_ARR_LEN] = {0};
  int i, size;

  size = sizeof (array) / sizeof (int);
  generate_rand_array (array, size);

  heap_sort (array, size);

  sort_array_dump (array, size, "Heap Sort");
}

void shell_sort_test (void)
{
  int array[SORT_ARR_LEN] = {0};
  int i, size;

  size = sizeof (array) / sizeof (int);
  generate_rand_array (array, size);

  shell_sort (array, size);

  sort_array_dump (array, size, "Shell Sort");
}

void merge_sort_test (void)
{
  int array[SORT_ARR_LEN] = {0};
  int i, size;

  size = sizeof (array) / sizeof (int);
  generate_rand_array (array, size);

  merge_sort (array, size);

  sort_array_dump (array, size, "Merge Sort");
}

void quick_sort_test (void)
{
  int array[SORT_ARR_LEN] = {0};
  int i, size;

  size = sizeof (array) / sizeof (int);
  generate_rand_array (array, size);

  quick_sort (array, size);

  sort_array_dump (array, size, "Quick Sort");
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
  huffman_coding_test();

  printf ("\n******* Longest Common Subsequent's Algorithm ******** \n");
  char str1[] = "Giayeuminhchau";
  char str2[] = "Giaminhchau";
  lcs(str1, str2);

  printf ("\n**************** Sort Algorithms ***************** \n");
  measure_function_time_clock (bubble_sort_test);
  measure_function_time_clock (selection_sort_test);
  measure_function_time_clock (insertion_sort_test);
  measure_function_time_clock (count_sort_test);
  measure_function_time_clock (radix_sort_test);
  measure_function_time_clock (bucket_sort_test);
  measure_function_time_clock (heap_sort_test);
  measure_function_time_clock (shell_sort_test);
  measure_function_time_clock (merge_sort_test);
  measure_function_time_clock (quick_sort_test);

  return 0;
}