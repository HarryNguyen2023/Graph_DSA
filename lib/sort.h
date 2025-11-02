#ifndef __SORT_H__
#define __SORT_H__

typedef struct Bucket
{
  int size;
  int *bucket;
} Bucket;

void sort_array_dump (int *arr, int size, char *sort_name);
int bubble_sort (int *arr, int size);
int selection_sort (int *arr, int size);
int insertion_sort (int *arr, int size);
int count_sort (int *arr, int size, int **out_arr);
int radix_sort (int *arr, int size);
int bucket_sort (int *arr, int size);
int heap_sort (int *arr, int size);
int shell_sort (int *arr, int size);
int merge_sort (int *arr, int size);
int quick_sort (int *arr, int size);

#endif /* __SORT_H__ */