#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "priority_queue.h"
#include "binary_tree.h"
#include "huffman_code.h"
#include "hash_table.h"

#define MAX_TREE_HEIGHT     (10)

int char_in_str (char *s, char c, int len)
{
  int i = 0;

  if (! s || ! len)
    return 0;

  for (i = 0; i < len; ++i)
    if (s[i] == c)
      return i;

  return 0;
}

int huffman_data_cmp (void *h1, void *h2)
{
  BinTreeNode *node1 = (BinTreeNode *)h1;
  BinTreeNode *node2 = (BinTreeNode *)h2;
  HuffmanData *data1 = (HuffmanData *)(node1->data);
  HuffmanData *data2 = (HuffmanData *)(node2->data);

  if (! data1 && ! data2)
    return 0;
  
  if (! data1 || ! data2)
    return -1;

  if (data1->freq > data2->freq)
    return 1;
  else if (data1->freq < data2->freq)
    return -1;
  else
  {
    if (data1->item > data2->item)
      return 1;
    else if (data1->item < data2->item)
      return -1;
    else
      return 0;
  }

  return 0;
}

HuffmanData *huffman_data_create (char item, int freq)
{
  HuffmanData *node = (HuffmanData *)malloc(sizeof(HuffmanData));
  if (! node)
    return NULL;

  node->item  = item;
  node->freq  = freq;
  return node;
}

void huffman_data_dump (void *node)
{
  HuffmanData *node_data = (HuffmanData *)node;
  if (! node_data)
    return;

  printf ("Item:%c - Count: %d\n", node_data->item, node_data->freq);
}

void huffman_code_print_prefix_code(char arr[], int n)
{
  int i;
  for (i = 0; i < n; ++i)
    printf("%c", arr[i]);

  printf("\n");
}

void huffman_code_tree_print (BinTreeNode *root, char arr[], int top)
{
  if (root->left)
  {
    arr[top] = '0';
    huffman_code_tree_print (root->left, arr, top + 1);
  }

  if (root->right)
  {
    arr[top] = '1';
    huffman_code_tree_print (root->right, arr, top + 1);
  }

  if (bin_tree_is_leaf_node(root))
  {
    if (root->bin_tree_node_dump)
      (*root->bin_tree_node_dump)(root->data);
    huffman_code_print_prefix_code(arr, top);
  }
}

void huffman_prefix_code_dump (void *data)
{
  char *prefix_code = (char *)data;
  if (prefix_code)
  {
    printf ("%s\n", prefix_code);
  }
  return;
}

int huffman_code_store_util (HashTable *table, char arr[], int top, void *data)
{
  char *prefix_code, *prefix_key;

  if (! table || ! table->table || ! table->range || ! arr)
    return -1;

  HuffmanData *node_data = (HuffmanData *)data;
  if (! node_data)
    return -1;

  prefix_code = (char *)malloc((top + 1) * sizeof(char));
  if (! prefix_code)
  {
    printf ("[%s,%d] Fail to allocate memory for prefix code\n", __func__, __LINE__);
    return -1;
  }
  memset (prefix_code, 0, (top + 1) * sizeof(char));
  strncpy (prefix_code, arr, top);

  prefix_key = (char *)malloc(2 * sizeof(char));
  if (! prefix_key)
  {
    printf ("[%s,%d] Fail to allocate memory for prefix key\n", __func__, __LINE__);
    if (prefix_code) 
      free(prefix_code);
    return -1;
  }
  memset (prefix_key, 0, 2 * sizeof(char));
  prefix_key[0] = node_data->item;
  prefix_key[1] = '\0';

  return hash_tbl_insert (table, (const char*)prefix_key, 2, prefix_code, huffman_prefix_code_dump);
}

void huffman_code_store (BinTreeNode *root, char arr[], int top, HashTable *table)
{
  if (root->left)
  {
    arr[top] = '0';
    huffman_code_store (root->left, arr, top + 1, table);
  }

  if (root->right)
  {
    arr[top] = '1';
    huffman_code_store (root->right, arr, top + 1, table);
  }

  if (bin_tree_is_leaf_node(root))
  {
    huffman_code_store_util (table, arr, top, root->data);
  }
}

BinTreeNode *huffman_coding_util (char *symbol, int *freq, int symbol_num, HuffmanData **huffman_data)
{
  PriorityQueue *pq = NULL;
  HuffmanData *temp = NULL;
  BinTreeNode *left = NULL, *right = NULL, *top = NULL, *node = NULL;
  int i;

  if (! symbol || ! freq || ! symbol_num || ! huffman_data)
    return NULL;

  pq = pq_create (symbol_num, huffman_data_cmp, NULL);
  if (! pq)
  {
    printf ("[%s,%d] Fail to create priority queue\n", __func__, __LINE__);
    return NULL;
  }

  *huffman_data = (HuffmanData *)malloc((symbol_num) * sizeof (HuffmanData));
  if (! *huffman_data)
  {
    printf ("[%s,%d] Fail to create list of hufman data\n", __func__, __LINE__);
    pq_deinit (pq);
    return NULL;
  }

  for (i = 0; i < symbol_num; ++i)
  {
    (*huffman_data)[i].freq  = freq[i];
    (*huffman_data)[i].item  = symbol[i];
    node = bin_tree_node_create ((void *)&(*huffman_data)[i], huffman_data_dump);
    if (! node)
    {
      printf ("[%s,%d] Fail to create Binary Tree node\n", __func__, __LINE__);
      break;
    }
    pq_add (pq, (void *)node);
  }

  while (pq->size > 1)
  {
    left  = (BinTreeNode *)pq_extract_top (pq);
    right = (BinTreeNode *)pq_extract_top (pq);

    if (left && right)
      top = bin_tree_node_create (huffman_data_create ('$', ((HuffmanData *)(left->data))->freq + ((HuffmanData *)(right->data))->freq), huffman_data_dump);
    else
    {
      printf ("[%s,%d] Fail to get 2 min node from priority queue\n", __func__, __LINE__);
      break;
    }

    top->left   = left;
    top->right  = right;

    pq_add(pq, (void *)top);
  }

  top = (BinTreeNode *)pq_extract_top (pq);

  pq_deinit (pq);
  return top;
}

int huffman_encoding (char *s, int len, char **out_str, int *out_str_len, BinTreeNode **root, HuffmanData **huffman_data)
{
  char *symbol = NULL;
  int *freq = NULL, symbol_num = 0, i, c_index;
  HashTable *table = NULL;
  char *arr = NULL, *prefix_code = NULL, key[2];

  if (! s || ! len)
    return -1;

  /* Get the number of appearance of each character */
  for (i = 0; i < len; ++i)
  {
    if (! (c_index = char_in_str (symbol, s[i], symbol_num)))
    {
      symbol  = (char *)realloc(symbol, (++symbol_num) * sizeof (char));
      freq    = (int *)realloc(freq, symbol_num * sizeof(int));
      symbol[symbol_num - 1]  = s[i];
      freq[symbol_num - 1]    = 1;
    }
    else
    {
      if (c_index >= 0)
        freq[c_index]++;
    }
  }

  arr = (char *)calloc (MAX_TREE_HEIGHT + 1, sizeof(char));
  if (! arr)
  {
    printf ("[%s,%d] Fail to create prefix code array\n", __func__, __LINE__);
    goto EXIT;
  }
  memset (arr, 0, (MAX_TREE_HEIGHT + 1) * sizeof (char));

  table = hash_tbl_create ((unsigned int)symbol_num * 3);
  if (! table)
  {
    printf ("[%s,%d] Fail to create prefix code hash table\n", __func__, __LINE__);
    goto EXIT;
  }

  *root = huffman_coding_util (symbol, freq, symbol_num, huffman_data);
  if (! *root)
  {
    printf ("[%s,%d] Fail to build huffman code tree\n", __func__, __LINE__);
    goto EXIT;
  }

  /* Store the symbol and its associated prefix code */
  huffman_code_store (*root, arr, 0, table);

  *out_str_len  = 0;
  *out_str      = NULL;
  for (i = 0; i < len; ++i)
  {
    key[0] = s[i];
    key[1] = '\0';
    if (hash_tbl_get (table, key, (void *)&prefix_code) == 0)
    {
      *out_str_len += strlen (prefix_code);
      *out_str = (char *)realloc(*out_str, (*out_str_len + 1) * sizeof (char));
      memcpy (*out_str + *out_str_len - strlen (prefix_code), prefix_code, strlen (prefix_code) + 1);
    }
  }

EXIT:
  if (symbol) free (symbol);
  symbol = NULL;
  if (freq) free(freq);
  freq = NULL;
  if (arr)  free(arr);
  arr = NULL;
  if (table) hash_tbl_delete (table);
  return 0;
}

void huffman_decoding_util (BinTreeNode *root, char* encoded_str, int *prefix_len, char **decoded_str, int *decoded_str_len)
{
  char symbol[2] = {0};

  if (! root || ! encoded_str || ! prefix_len)
    return;

  if (bin_tree_is_leaf_node(root))
  {
    HuffmanData *node_data = (HuffmanData *)root->data;
    if (! node_data)
      return;

    (*decoded_str_len)++;
    *decoded_str = (char *)realloc(*decoded_str, (*decoded_str_len + 1) * sizeof(char));
    symbol[0] = node_data->item;
    symbol[1] = '\0';
    memcpy(*decoded_str + (*decoded_str_len) - 1, symbol, 2);
    return;
  }

  if (*encoded_str == '0')
  {
    (*prefix_len)++;
    return huffman_decoding_util (root->left, ++encoded_str, prefix_len, decoded_str, decoded_str_len);
  }

  if (*encoded_str == '1')
  {
    (*prefix_len)++;
    return huffman_decoding_util (root->right, ++encoded_str, prefix_len, decoded_str, decoded_str_len);
  }
}

void huffman_decoding (BinTreeNode *root, char* encoded_str, int encoded_str_len, char **decoded_str, int* decoded_str_len)
{
  int prefix_len = 0;

  if (! root || ! encoded_str || encoded_str_len <= 0)
    return;

  if (prefix_len >= encoded_str_len)
    return;

  huffman_decoding_util (root, encoded_str, &prefix_len, decoded_str, decoded_str_len);
  huffman_decoding (root, encoded_str + prefix_len, encoded_str_len - prefix_len, decoded_str, decoded_str_len);
}