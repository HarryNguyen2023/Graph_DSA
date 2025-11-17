#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"
#include "b_tree.h"
#include "b_plus_tree.h"

BPlusTreeNode* bp_tree_node_create (int t)
{
  BPlusTreeNode* new_node = NULL;

  new_node = (BPlusTreeNode *)malloc(sizeof (BPlusTreeNode));
  if (! new_node)
  {
    printf ("[%s,%d] Fail to allocate memory for B+ Tree node", __func__, __LINE__);
    return NULL;
  }

  new_node->is_leaf = 1;
  new_node->parent  = NULL;
  new_node->key     = (int *) malloc ((2 * t + 1) * sizeof (int));
  if (! new_node->key)
  {
    printf ("[%s,%d] Fail to allocate memory for B Tree node key list", __func__, __LINE__);
    goto ERR_EXIT;
  }
  memset (new_node->key, 0, (2 * t + 1) * sizeof (int));

  new_node->childs  = (BPlusTreeNode **)malloc((2 * t + 2) * sizeof (BPlusTreeNode *));
  if (! new_node->childs)
  {
    printf ("[%s,%d] Fail to allocate memory for B Tree node childs list", __func__, __LINE__);
    goto ERR_EXIT;
  }
  memset (new_node->childs, 0, (2 * t + 2) * sizeof (BPlusTreeNode *));

  new_node->data  = (void **)malloc((2 * t + 1) * sizeof (void *));
  if (! new_node->data)
  {
    printf ("[%s,%d] Fail to allocate memory for B Tree node data", __func__, __LINE__);
    goto ERR_EXIT;
  }
  memset (new_node->data, 0, (2 * t + 1) * sizeof (void *));
  new_node->count   = 0;
  new_node->next    = NULL;

  return new_node;

ERR_EXIT:
  if (*(new_node->childs))  free (*(new_node->childs));
  *(new_node->childs) = NULL;
  if (new_node->key)  free (new_node->key);
  new_node->key = NULL;
  if (new_node) free (new_node);
  new_node = NULL;
  return NULL;
}

void bp_tree_node_delete (BPlusTreeNode *node)
{
  if (! node)
    return;

  // if (node->key)  free (node->key);
  // node->key = NULL;
  // if (*(node->childs)) free (*(node->childs));
  // *(node->childs) = NULL;
  // if (*(node->data))  free (*(node->data));
  // *(node->data) = NULL;
  free (node);
  node = NULL;
}

BPlusTree* bp_tree_create (int t)
{
  BPlusTree* tree = NULL;
  tree = (BPlusTree *)malloc(sizeof (BPlusTree));
  if (! tree)
  {
    printf ("[%s,%d] Fail to allocate memory for B+ Tree", __func__, __LINE__);
    return NULL;
  }

  tree->root = NULL;
  tree->size = 0;
  tree->t    = t;

  return tree;
}

void bp_tree_delete_util (BPlusTreeNode *root)
{
  int i;

  if (! root)
    return;

  if (root->is_leaf)
    return bp_tree_node_delete (root);

  for (i = 0; i < root->count + 1; ++i)
  {
    bp_tree_delete_util (root->childs[i]);
  }

  bp_tree_node_delete (root);
}

void bp_tree_delete (BPlusTree *tree)
{
  if (! tree)
    return;

  if (tree->root)
    bp_tree_delete_util (tree->root);

  free (tree);
  tree = NULL;
}

BPlusTreeNode* bp_tree_search_util (BPlusTreeNode *root, int key, int *index)
{
  int i = 0;

  if (! root)
    return NULL;

  while (i < root->count && key > root->key[i])
    i++;

  if (root->is_leaf)
  {
    if (key == root->key[i])
    {
      *index = i;
      return root;
    }
    else
    {
      *index = -1;
      return NULL;
    }
  }
  else
  {
    if (key == root->key[i])
      i++;
  }

  return bp_tree_search_util (root->childs[i], key, index);
}

BPlusTreeNode* bp_tree_search (BPlusTree *tree, int key, int *index)
{
  if (! tree || ! tree->size || ! tree->root)
    return NULL;

  return bp_tree_search_util (tree->root, key, index);
}

BPlusTreeNode *bp_tree_insert_first_node (int t, int key, void *data,
                                          BPlusTreeNode* frist_child, BPlusTreeNode* sec_child)
{
  BPlusTreeNode* new_node = NULL;

  new_node = bp_tree_node_create (t);
  if (! new_node)
  {
    printf ("[%s,%d] Fail to create the new node of B+ Tree\n", __func__, __LINE__);
    return NULL;
  }

  new_node->key[0] = key;
  new_node->data[0] = data;
  new_node->childs[0] = frist_child;
  if (frist_child)
    frist_child->parent = new_node;
  new_node->childs[1] = sec_child;
  if (sec_child)
    sec_child->parent = new_node;

  if (frist_child || sec_child)
    new_node->is_leaf = 0;
  new_node->count++;

  return new_node;
}

BPlusTreeNode *bp_tree_insert_node (BPlusTreeNode *node, int key, void *data, BPlusTreeNode *child)
{
  int i = 0, j = 0;

  if (! node)
    return NULL;

  while (i < node->count && key > node->key[i])
    i++;

  for (j = node->count; j > i; --j)
  {
    node->key[j] = node->key[j - 1];
    node->data[j] = node->data[j - 1];
    if (child)
      node->childs[j + 1] = node->childs[j];
  }

  node->key[i] = key;
  node->data[i] = data;
  if (child)
  {
    node->childs[i + 1] = child;
    child->parent = node;
  }
  node->count++;

  return node;
}

BPlusTreeNode* bp_tree_split_childs (BPlusTreeNode **root, BPlusTreeNode *node, int t)
{
  BPlusTreeNode *sib = NULL, *parent = NULL;
  int i = 0, med_key;

  if (! node)
    return NULL;

  med_key = node->key[t];
  if (! node->is_leaf)
  {
    node->key[t] = 0;
    node->count--;
  }
  /* Move the last t + 1 keys to the sibling */
  i = (node->is_leaf) ? t : (t + 1);
  while (i < 2 * t + 1)
  {
    if (! sib)
    {
      sib = bp_tree_insert_first_node (t, node->key[i], node->data[i], node->childs[i], node->childs[i + 1]);
      if (! sib)
      {
        printf ("[%s,%d] Fail to allocate memory for sibling node\n", __func__, __LINE__);
        return NULL;
      }
      node->next = sib;
    }
    else
      bp_tree_insert_node (sib, node->key[i], node->data[i], node->childs[i + 1]);

    node->key[i]    = 0;
    node->data[i]   = NULL;
    node->childs[i] = NULL;
    node->childs[i + 1] = NULL;
    node->count--;
    i++;
  }

  if (! node->parent)
  {
    parent = bp_tree_insert_first_node (t, med_key, NULL, node, sib);
    if (! parent)
    {
      printf ("[%s,%d] Fail to allocate memory for parent node\n", __func__, __LINE__);
      return NULL;
    }
    *root = parent;
  }
  else
  {
    parent = bp_tree_insert_node (node->parent, med_key, NULL, sib);
    if (parent->count > 2 * t)
      return bp_tree_split_childs (root, parent, t);
  }

  return parent;
}

BPlusTreeNode* bp_tree_insert_util (BPlusTreeNode **root, BPlusTreeNode *node, int t, int key, void *data)
{
  BPlusTreeNode* new_node = NULL;
  int i = 0, j = 0;

  if (! *root)
    return bp_tree_insert_first_node (t, key, data, NULL, NULL);

  while (i < node->count && key >= node->key[i])
    i++;

  if (node->is_leaf)
  {
    node = bp_tree_insert_node (node, key, data, NULL);
    if (node->count > 2 * t)
      return bp_tree_split_childs (root, node, t);

    return node;
  }

  return bp_tree_insert_util (root, node->childs[i], t, key, data);;
}

BPlusTreeNode* bp_tree_insert (BPlusTree *tree, int key, void *data)
{
  BPlusTreeNode *root = NULL, *temp = NULL;
  int index = 0;

  if (! tree)
    return NULL;

  temp = bp_tree_search (tree, key, &index);
  if (temp && (index >= 0 && index < temp->count)
      && temp->key[index] == key)
  {
    printf ("Node %d has already exist in the B+ Tree\n", key);
    return temp;
  }

  root = bp_tree_insert_util (&(tree->root), tree->root, tree->t, key, data);
  if (root)
  {
    if (tree->root == NULL)
      tree->root = root;
    tree->size++;
  }

  return root;
}

void bp_tree_print_2d (BPlusTree *tree)
{
  if (! tree || ! tree->root)
    return;

  b_tree_print_2d ((BTree *)tree);
}