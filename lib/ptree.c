#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ptree.h"

unsigned char maskbit[8] = {0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE};

struct ptree *
ptree_init (unsigned int max_key_length)
{
  struct ptree *tree = NULL;

  tree = (struct ptree *)malloc (sizeof (struct ptree));
  if (tree == NULL)
  {
    printf ("[%s,%d] Fail to allocate memory for new PTree\n", __func__, __LINE__);
    return NULL;
  }

  memset (tree, 0, sizeof (struct ptree));
  tree->max_key_length = max_key_length;
  return tree;
}

struct ptree_node *
ptree_lock_node (struct ptree_node *node)
{
  if (! node)
    return NULL;

  (node->lock)++;
  return node;
}

void
ptree_unlock_node (struct ptree_node *node)
{
  if (! node)
    return;

  if (--(node->lock) == 0)
    ptree_node_free (node);
}

struct ptree_node *
ptree_node_create (struct ptree *tree, unsigned char *key, unsigned int key_length)
{
  struct ptree_node *new_node = NULL;
  int byte_len;

  if (! tree || ! key || ! key_length || key_length > tree->max_key_length)
    return NULL;

  byte_len = (key_length / 8) + 1;
  new_node = (struct ptree_node *)malloc(sizeof (struct ptree_node) + byte_len);
  if (! new_node)
  {
    printf ("[%s,%d] Fail to allocate memory for new PTree node\n", __func__, __LINE__);
    return NULL;
  }
  memset(new_node, 0, sizeof (struct ptree_node) + byte_len);
  memcpy (new_node->key, key, byte_len);
  new_node->key_length = key_length;

  ptree_lock_node (new_node);
  return new_node;
}

void
ptree_node_free (struct ptree_node *node)
{
  if (! node || node->info || node->p_left || node->p_right)
    return;

  free (node);
  node = NULL;
}

void
ptree_delete_util (struct ptree_node *node)
{
  if (! node)
    return;

  if (node->p_left)
    ptree_delete_util (node->p_left);

  if (node->p_right)
    ptree_delete_util (node->p_right);

  ptree_node_free (node);
}

void
ptree_free (struct ptree *tree)
{
  if (! tree || ! tree->top)
    return;

  ptree_delete_util (tree->top);
  free (tree);
  tree = NULL;
}

int
ptree_key_match (unsigned char *key_1, unsigned int key_1_len,
                 unsigned char *key_2, unsigned int key_2_len)
{
  int offset, shift;

  if (key_1_len > key_2_len)
    return 0;

  offset = key_1_len / 8;
  shift  = key_1_len % 8;

  if (shift)
    if (maskbit[shift] & ((key_1[offset]) ^ (key_2[offset])))
      return 0;

  while (--offset >= 0)
    if (key_1[offset] != key_2[offset])
      return 0;

  return 1;
}

int
ptree_check_bit (unsigned char *key, unsigned int key_len)
{
  int offset, shift;

  offset = key_len / 8;
  shift  = key_len % 8;
  if (shift == 0)
    offset--;
  else
    shift  = 8 - shift;

  return (((key[offset] >> shift) & 1) ? 1 : 0);
}

struct ptree_node *
ptree_node_lookup (struct ptree *tree, unsigned char *key, unsigned int key_length)
{
  struct ptree_node *node = NULL;

  if (! tree || ! tree->top || ! key || key_length > tree->max_key_length)
    return NULL;

  node = tree->top;
  while (node && node->key_length <= key_length
        && ptree_key_match (node->key, node->key_length, key, key_length))
  {
    if (node->key_length == key_length)
      return ptree_lock_node (node);

    node = node->link[ptree_check_bit (key, node->key_length + 1)];
  }

  return NULL;
}

struct ptree_node *
ptree_node_set (struct ptree *tree, unsigned char *key, unsigned int key_length)
{
  struct ptree_node *node;

  if (! tree || key_length > tree->max_key_length)
    return NULL;

  node = ptree_node_create (tree, key, key_length);
  if (! node)
    return NULL;

  node->tree = tree;
  return node;
}

void
ptree_node_set_link (struct ptree_node *parent, struct ptree_node *node)
{
  if (! parent || ! node || parent->key_length >= node->key_length)
    return;

  node->parent = parent;
  parent->link[ptree_check_bit (node->key, parent->key_length + 1)] = node;
}

struct ptree_node *
ptree_node_common (unsigned char *n_key, unsigned int n_len, struct ptree_node *p_node)
{
  int i, j, min_key_len, key_len;
  int offset, shift;
  struct ptree_node *new_node = NULL;
  unsigned char *p_key, mask;
  unsigned int p_len;

  if (! n_key || ! p_node)
    return NULL;

  p_key = p_node->key;
  p_len = p_node->key_length;

  min_key_len = (n_len <= p_len) ? n_len : p_len;
  offset = min_key_len / 8;
  shift  = min_key_len % 8;
  if (shift)
    offset += 1;

  for (i = 0; i < offset; ++i)
    if (n_key[i] != p_key[i])
      break;

  key_len = i * 8;
  if (key_len < min_key_len)
  {
    mask = 0x80;
    while (key_len < min_key_len && (! (mask & (n_key[i] ^ p_key[i]))))
    {
      mask >>= 1;
      key_len++;
    }

    if (key_len < min_key_len)
    {
      new_node = ptree_node_set (p_node->tree, p_key, key_len);
      if (! new_node)
        return NULL;

      ptree_node_set_link (new_node, p_node);
      return new_node;
    }
  }

  new_node = ptree_node_set (p_node->tree, n_key, n_len);
  if (! new_node)
    return NULL;

  if (min_key_len == n_len)
    ptree_node_set_link (new_node, p_node);
  else
    printf ("[%s,%d] Error: Case invalid\n", __func__, __LINE__);

  return new_node;
}

struct ptree_node *
ptree_node_get (struct ptree *tree, unsigned char *key, unsigned int key_length)
{
  struct ptree_node *node = NULL, *match = NULL, *new_node = NULL;

  if (! tree || ! key || key_length > tree->max_key_length)
    return NULL;

  node = tree->top;
  while (node && node->key_length <= key_length
        && ptree_key_match (node->key, node->key_length, key, key_length))
  {
    if (node->key_length == key_length)
      return ptree_lock_node (node);

    match = node;
    node = node->link[ptree_check_bit(key, node->key_length + 1)];
  }

  if (node == NULL)
  {
    new_node = ptree_node_set (tree, key, key_length);
    if (! new_node)
      return NULL;

    if (match)
      ptree_node_set_link (match, new_node);
    else
      tree->top = new_node;
  }
  else
  {
    new_node = ptree_node_common (key, key_length, node);
    if (! new_node)
      return NULL;

    if (match)
      ptree_node_set_link (match, new_node);
    else
      tree->top = new_node;

    new_node->tree = tree;
    if (new_node->key_length != key_length)
    {
      match = new_node;
      new_node = ptree_node_set (tree, key, key_length);
      ptree_node_set_link (match, new_node);
    }
  }

  return new_node;
}

void
ptree_node_dump (struct ptree_node *node)
{
  if (! node)
    return;

  if (node->p_left)
    ptree_delete_util (node->p_left);

  if (node->p_right)
    ptree_delete_util (node->p_right);

  printf ("%s\n", node->key);
}

void
ptree_dump (struct ptree *tree)
{
  if (! tree || ! tree->top)
    return;

  ptree_node_dump (tree->top);
}

void
ptree_print_util (struct ptree_node *node, int count, int space)
{
  if (node == NULL)
      return;

  space += count;

  ptree_print_util (node->p_right, count, space);

  printf ("\n");
  for (int i = count; i < space; i++)
    printf (" ");
  printf ("%s", (char *)&node->key[0]);

  ptree_print_util (node->p_left, count, space);    
}

void
ptree_print (struct ptree *tree)
{
  if (! tree || ! tree->top)
    return;

  ptree_print_util (tree->top, 15, 0);
}

void
ptree_node_remove (struct ptree_node *node)
{
  if (! node || (node->p_left && node->p_right))
    return;

  if (node->p_left)
  {
    if (node->parent == NULL)
      node->tree->top = node->p_left;
    else if (node == node->parent->p_left)
      node->parent->p_left = node->p_left;
    else
      node->parent->p_right = node->p_left;

    node->p_left->parent = node->parent;
  }
  else if (node->p_right)
  {
    if (node->parent == NULL)
      node->tree->top = node->p_right;
    else if (node == node->parent->p_left)
      node->parent->p_left = node->p_right;
    else
      node->parent->p_right = node->p_right;

    node->p_right->parent = node->parent;
  }
  else
  {
    if (node->parent == NULL)
      node->tree->top = NULL;
    else if (node == node->parent->p_left)
      node->parent->p_left = NULL;
    else
      node->parent->p_right = NULL;
  }

  ptree_node_free (node);
  return;
}