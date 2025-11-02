#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "sort.h"

void swap (int *a, int *b)
{
  int temp;

  if (! a || ! b)
    return;

  temp  = *a;
  *a    = *b;
  *b    = temp;
}

void sort_array_dump (int *arr, int size, char *sort_name)
{
  int i;

  if (! arr || ! size || ! sort_name)
    return;

  printf ("%sed array: ", sort_name);
  for (i = 0; i < size; ++i)
    printf ("%d ", arr[i]);
  printf ("\n");
}

int get_max (int *arr, int size)
{
  int max = 0, i;

  if (! arr || ! size)
    return -1;

  max = arr[0];
  for (i = 1; i < size; ++i)
    if (arr[i] > max)
      max = arr[i];

  return max;
}

int bubble_sort (int *arr, int size)
{
  int i, j, swapped = 0;

  if (! arr || ! size)
    return -1;

  for (i = 0; i < size - 1; ++i)
  {
    for (j = 0; j < size - i - 1; ++j)
    {
      if (arr[j] > arr[j + 1])
      {
        swap (&arr[j], &arr[j + 1]);
        swapped = 1;
      }
    }
    if (! swapped)
      break;
  }

  return 0;
}

int selection_sort (int *arr, int size)
{
  int i, j, min_indx;

  if (! arr || ! size)
    return -1;

  for (i = 0; i < size; ++i)
  {
    min_indx = i;
    for (j =  i + 1; j < size; ++j)
    {
      if (arr[j] < arr[min_indx])
        min_indx = j;
    }
    if (min_indx != i)
      swap (&arr[min_indx], &arr[i]);
  }

  return 0;
}

int insertion_sort (int *arr, int size)
{
  int i, j, key;

  if (! arr || ! size)
    return -1;

  for (i = 1; i < size; ++i)
  {
    key = arr[i];
    j = i - 1;
    while (j >= 0 && key < arr[j])
    {
      arr[j + 1] = arr[j];
      j--;
    }
    arr[j + 1] = key;
  }

  return 0;
}

int count_sort (int *arr, int size, int **out_arr)
{
  int *freq_arr, max, old_max, i, rv = 0;

  if (! arr || ! size)
    return -1;

  max = arr[0];
  freq_arr = (int *)malloc((max + 1) * sizeof(int));
  if (! freq_arr)
  {
    printf ("[%s,%d] Fail to allocate memory for character frequency array\n",
            __func__, __LINE__);
    return -1;
  }
  memset (freq_arr, 0, (max + 1) * sizeof(int));
  freq_arr[max]++;

  for (i = 1; i < size; ++i)
  {
    if (arr[i] > max)
    {
      old_max = max;
      max     = arr[i];

      freq_arr = (int *)realloc(freq_arr, (max + 1) * sizeof(int));
      if (! freq_arr)
      {
        printf ("[%s,%d] Fail to allocate memory for character frequency array\n",
              __func__, __LINE__);
        return -1;
      }
      memset (freq_arr + old_max + 1, 0, (max - old_max) * sizeof (int));
    }
    freq_arr[arr[i]]++;
  }

  for (i = 1; i < max + 1; ++i)
    freq_arr[i] += freq_arr[i - 1];

  *out_arr = (int *)malloc(size * sizeof(int));
  if (! *out_arr)
  {
    printf ("[%s,%d] Fail to allocate memory for output array\n",
              __func__, __LINE__);
    rv = -1;
    goto EXIT;
  }
  memset (*out_arr, 0, size * sizeof(int));

  for (i = size - 1; i >= 0; --i)
  {
    (*out_arr)[freq_arr[arr[i]] - 1] = arr[i];
    freq_arr[arr[i]]--;
  }

EXIT:
  if (freq_arr) free (freq_arr);
  freq_arr = NULL;
  return rv;
}

int bubble_sort_radix (int *arr, int size, int place)
{
  int i, j, swapped = 0;

  if (! arr || ! size)
    return -1;

  for (i = 0; i < size - 1; ++i)
  {
    for (j = 0; j < size - i - 1; ++j)
    {
      if (((arr[j] / place) % 10) > ((arr[j + 1] / place) % 10))
      {
        swap (&arr[j], &arr[j + 1]);
        swapped = 1;
      }
    }
    if (! swapped)
      break;
  }

  return 0;
}

int radix_sort (int *arr, int size)
{
  int max, i;

  if (! arr || ! size)
    return -1;

  max = get_max (arr, size);
  if (max == -1)
    return -1;

  for (i = 1; (max / i) > 0; i *= 10)
    bubble_sort_radix (arr, size, i);

  return 0;
}

int bucket_sort (int *arr, int size)
{
  Bucket *bucket_list;
  int i, div, rv = 0;

  if (! arr || ! size)
    return -1;

  bucket_list = (Bucket *)malloc(11 * sizeof (Bucket));
  if (! bucket_list)
  {
    printf ("[%s,%d] Fail to allocate memory for bucket list\n", __func__, __LINE__);
    return -1;
  }

  for (i = 0; i < 11; ++i)
  {
    bucket_list[i].bucket = NULL;
    bucket_list[i].size   = 0;
  }

  for (i = 0; i < size; ++i)
  {
    div = arr[i] / 10;
    if (div > 9)
      div = 10;
    else if (div < 0)
      div = 0;

    bucket_list[div].bucket = (int *)realloc(bucket_list[div].bucket, ++(bucket_list[div].size) * sizeof (int));
    if (! bucket_list[div].bucket)
    {
      printf ("[%s,%d] Fail to allocate memory for bucket %d\n", __func__, __LINE__, div);
      rv = -1;
      goto EXIT;
    }
    bucket_list[div].bucket[bucket_list[div].size - 1] = arr[i];
  }

  memset (arr, 0, size * sizeof(int));
  size = 0;

  for (i = 0; i < 11; ++i)
  {
    if (bucket_list[i].bucket && bucket_list[i].size)
    {
      selection_sort (bucket_list[i].bucket, bucket_list[i].size);
      memcpy (arr + size, bucket_list[i].bucket, bucket_list[i].size * sizeof (int));
      size += bucket_list[i].size;
    }
  }

EXIT:
  for (i = 0; i < 11; ++i)
  {
    if (bucket_list[i].bucket)
      free (bucket_list[i].bucket);
    bucket_list[i].bucket = NULL;
  }
  if (bucket_list)  free (bucket_list);
  bucket_list = NULL;
  return rv;
}

int heap_sort_heapify (int *arr, int size, int i)
{
  int largest = i, left, right;

  if (! arr || ! size)
    return -1;

  left  = 2 * i + 1;
  right = 2 * i + 2;
  if (left < size && arr[left] > arr[largest])
    largest = left;
  if (right < size && arr[right] > arr[largest])
    largest = right;

  if (largest != i)
  {
    swap (&arr[i], &arr[largest]);
    heap_sort_heapify (arr, size, largest);
  }

  return 0;
}

int heap_sort (int *arr, int size)
{
  int i;

  if (! arr || ! size)
    return -1;

  /* Build max heap */
  for (i = size / 2 - 1; i >= 0; i--)
    heap_sort_heapify (arr, size, i);

  /* Do heap sort */
  for (i = size - 1; i >= 0; i--)
  {
    swap (&arr[0], &arr[i]);

    heap_sort_heapify (arr, i, 0);
  }

  return 0;
}

int shell_sort (int *arr, int size)
{
  int i, j, interval, key, swapped = 0;

  if (! arr || ! size)
    return -1;

  for (interval = size / 2; interval >= 1; interval /= 2)
  {
    for (i = interval; i < size; i++)
    {
      key = arr[i];
      j = i;
      while (j >= interval && arr[j - interval] > key)
      {
        arr[j] = arr[j - interval];
        j -= interval;
      }
      arr[j] = key;
    }
  }

  return 0;
}

void merge_array (int *arr, int start, int mid, int end)
{
  int *merge_arr = NULL, i, j, k;

  if (! arr || start > end || start > mid || mid > end)
    return;

  merge_arr = (int *)malloc((end - start + 1) * sizeof (int));
  if (! merge_arr)
  {
    printf ("[%s,%d] Fail to allocate memory for the merge array", __func__, __LINE__);
    return;
  }
  memset (merge_arr, 0, (end - start + 1) * sizeof (int));

  i = start;
  j = mid + 1;
  k = 0;
  while (i <= mid && j <= end)
  {
    if (arr[i] < arr[j])
      merge_arr[k++] = arr[i++];
    else if (arr[j] < arr[i])
      merge_arr[k++] = arr[j++];
    else
    {
      merge_arr[k++] = arr[i++];
      merge_arr[k++] = arr[j++];
    }
  }

  while (i <= mid)
  {
    merge_arr[k++] = arr[i++];
  }

  while (j <= end)
  {
    merge_arr[k++] = arr[j++];
  }

  memcpy (arr + start, merge_arr, (end - start + 1) * sizeof (int));

  if (merge_arr)  free (merge_arr);
  merge_arr = NULL;
  return;
}

void merge_sort_util (int *arr, int start, int end)
{
  int mid;

  if (! arr || start >= end)
    return;

  mid = (start + end - 1) / 2;
  merge_sort_util (arr, start, mid);
  merge_sort_util (arr, mid + 1, end);
  merge_array (arr, start, mid, end);
}

int merge_sort (int *arr, int size)
{
  if (! arr || ! size)
    return -1;

  merge_sort_util (arr, 0, size - 1);
  return 0;
}

int quick_sort_partition (int *arr, int start, int end)
{
  int pivot, i, j;

  if (! arr || start >= end)
    return -1;

  pivot = arr[end];
  i     = (start - 1);

  for (j = start; j < end; ++j)
  {
    if (arr[j] < pivot)
    {
      i++;
      swap (&arr[i], &arr[j]);
    }
  }
  swap (&arr[i + 1], &arr[end]);
  return (i + 1);
}

int quick_sort_util (int *arr, int start, int end)
{
  int pivot;

  if (start < end)
  {
    pivot = quick_sort_partition (arr, start, end);
    quick_sort_util (arr, start, pivot - 1);
    quick_sort_util (arr, pivot + 1, end);
  }
  return 0;
}

int quick_sort (int *arr, int size)
{
  if (! arr || ! size)
    return -1;

  quick_sort_util (arr, 0, size - 1);
  return 0;
}