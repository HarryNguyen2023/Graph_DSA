#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"
#include "b_tree.h"

BTreeNode* b_tree_node_create (int t)
{
  BTreeNode* new_node = NULL;

  new_node = (BTreeNode *)malloc(sizeof (BTreeNode));
  if (! new_node)
  {
    printf ("[%s,%d] Fail to allocate memory for B Tree node", __func__, __LINE__);
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

  new_node->childs  = (BTreeNode **)malloc((2 * t + 2) * sizeof (BTreeNode *));
  if (! new_node->childs)
  {
    printf ("[%s,%d] Fail to allocate memory for B Tree node childs list", __func__, __LINE__);
    goto ERR_EXIT;
  }
  memset (new_node->childs, 0, (2 * t + 2) * sizeof (BTreeNode *));

  new_node->data  = (void **)malloc((2 * t + 1) * sizeof (void *));
  if (! new_node->data)
  {
    printf ("[%s,%d] Fail to allocate memory for B Tree node data", __func__, __LINE__);
    goto ERR_EXIT;
  }
  memset (new_node->data, 0, (2 * t + 1) * sizeof (void *));
  new_node->count   = 0;

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

void btree_node_delete (BTreeNode *node)
{
  if (! node)
    return;

  if (node->key)  free (node->key);
  node->key = NULL;
  if (*(node->childs)) free (*(node->childs));
  *(node->childs) = NULL;
  if (*(node->data))  free (*(node->data));
  *(node->data) = NULL;
  free (node);
  node = NULL;
}

BTree* b_tree_create (int t)
{
  BTree* tree = NULL;
  tree = (BTree *)malloc(sizeof (BTree));
  if (! tree)
  {
    printf ("[%s,%d] Fail to allocate memory for B Tree", __func__, __LINE__);
    return NULL;
  }

  tree->root = NULL;
  tree->size = 0;
  tree->t    = t;

  return tree;
}

void b_tree_delete_util (BTreeNode *root)
{
  int i;

  if (! root)
    return;

  if (root->is_leaf)
    return btree_node_delete (root);

  for (i = 0; i < root->count + 1; ++i)
  {
    b_tree_delete_util (root->childs[i]);
  }

  btree_node_delete (root);
}

void b_tree_delete (BTree *tree)
{
  if (! tree)
    return;

  if (tree->root)
    b_tree_delete_util (tree->root);

  free (tree);
  tree = NULL;
}

BTreeNode* b_tree_search_util (BTreeNode *root, int key, int *index)
{
  int i = 0;

  if (! root)
    return NULL;

  while (i < root->count && key > root->key[i])
    i++;

  if (key == root->key[i])
  {
    *index = i;
    return root;
  }

  if (root->is_leaf)
  {
    *index = -1;
    return NULL;
  }

  return b_tree_search_util (root->childs[i], key, index);
}

BTreeNode* b_tree_search (BTree *tree, int key, int *index)
{
  if (! tree || ! tree->size || ! tree->root)
    return NULL;

  return b_tree_search_util (tree->root, key, index);
}

BTreeNode *b_tree_insert_first_node (int t, int key, void *data,
                                     BTreeNode* frist_child, BTreeNode* sec_child)
{
  BTreeNode* new_node = NULL;

  new_node = b_tree_node_create (t);
  if (! new_node)
  {
    printf ("[%s,%d] Fail to create the new node of BTree\n", __func__, __LINE__);
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

BTreeNode *b_tree_insert_node (BTreeNode *node, int key, void *data, BTreeNode *child)
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
    node->childs[j + 1] = node->childs[j];
  }

  node->key[i] = key;
  node->data[i] = data;
  node->childs[i + 1] = child;
  if (child)
    child->parent = node;
  node->count++;

  return node;
}

BTreeNode* b_tree_split_childs (BTreeNode **root, BTreeNode *node, int t)
{
  BTreeNode *sib = NULL, *parent = NULL;
  int i = 0, j = 0, median;
  void *median_data = NULL;

  if (! node)
    return NULL;
  
  /* Move the last t keys to the sibling */
  for (i = t + 1; i < 2 * t + 1; ++i)
  {
    if (! sib)
    {
      sib = b_tree_insert_first_node (t, node->key[i], node->data[i], node->childs[i], node->childs[i + 1]);
      if (! sib)
      {
        printf ("[%s,%d] Fail to allocate memory for sibling node\n", __func__, __LINE__);
        return NULL;
      }
    }
    else
      b_tree_insert_node (sib, node->key[i], node->data[i], node->childs[i + 1]);

    node->key[i] = 0;
    node->data[i] = NULL;
    node->childs[i] = NULL;
    node->childs[i + 1] = NULL;
    node->count--; 
  }

  median = node->key[t];
  median_data = node->data[t];
  node->key[i] = 0;
  node->data[i] = NULL;
  node->count--;
  if (! node->parent)
  {
    parent = b_tree_insert_first_node (t, median, median_data, node, sib);
    if (! parent)
    {
      printf ("[%s,%d] Fail to allocate memory for parent node\n", __func__, __LINE__);
      return NULL;
    }
    *root = parent;
  }
  else
  {
    parent = b_tree_insert_node (node->parent, median, median_data, sib);
    if (parent->count > 2 * t)
      return b_tree_split_childs (root, parent, t);
  }

  return parent;
}

BTreeNode* b_tree_insert_util (BTreeNode **root, BTreeNode *node, int t, int key, void *data)
{
  BTreeNode* new_node = NULL;
  int i = 0, j = 0;

  if (key == 59)
    printf("\n");

  if (! node)
    return b_tree_insert_first_node (t, key, data, NULL, NULL);

  while (i < node->count && key > node->key[i])
    i++;

  if (key == node->key[i])
    return node;

  if (node->is_leaf)
  {
    node = b_tree_insert_node (node, key, data, NULL);
    if (node->count > 2 * t)
      return b_tree_split_childs (root, node, t);

    return node;
  }

  return b_tree_insert_util (root, node->childs[i], t, key, data);;
}

BTreeNode* b_tree_insert (BTree *tree, int key, void *data)
{
  BTreeNode *root = NULL;

  if (! tree)
    return NULL;

  root = b_tree_insert_util (&(tree->root), tree->root, tree->t, key, data);
  if (root)
  {
    if (tree->root == NULL)
      tree->root = root;
    tree->size++;
  }

  return root;
}

BTreeQueueNode *btree_queue_node_create (BTreeNode *tree_node, int level)
{
  BTreeQueueNode *node = (BTreeQueueNode *)malloc(sizeof (BTreeQueueNode));
  if (! node)
    return NULL;

  node->node = tree_node;
  node->level = level;
  return node;
}

void btree_print_2d_util (BTreeNode *root)
{
  Queue *q = NULL;
  BTreeQueueNode *qn = NULL;
  BTreeNode *node = NULL;
  int level = 0, cur_level = 0;;

  if (! root)
  { printf("<empty tree>\n");
    return;
  }

  /* compute height */
  int height = 0;
  {
    q = queue_create ();
    if (! q) return;

    queue_enqueue (q, btree_queue_node_create (root, 0));
    while (! queue_is_empty (q))
    {
      queue_dequeue (q, (void **)&qn);
      if (! qn)
        continue;
      level = qn->level;
      node = qn->node;
      if (level > height) height = level;
      if (! node->is_leaf)
      {
        for (int i = 0; i <= node->count; ++i)
          if (node->childs[i])
            queue_enqueue (q, btree_queue_node_create(node->childs[i], level + 1));
      }

      free (qn);  qn = NULL;
    }
    queue_delete (q); q = NULL;
  }

  /* print level by level */
  q = queue_create ();
  queue_enqueue (q, btree_queue_node_create (root, 0));

  while (! queue_is_empty(q))
  {
    queue_dequeue (q, (void **)&qn);
    if (! qn)
      continue;
    level = qn->level;
    node = qn->node;

    if (level != cur_level)
    {
      printf("\n");
      cur_level = level;
    }

    /* indent according to height */
    int indent = (height - level) * 10;
    printf("%*s", indent, "");

    /* print keys */
    for (int i = 0; i < node->count; ++i) {
      printf("%d ", node->key[i]);
    }
    printf("  ");   /* small gap before next node */

    /* enqueue children */
    if (! node->is_leaf) {
      for (int i = 0; i <= node->count; ++i)
        if (node->childs[i])
          queue_enqueue(q, btree_queue_node_create(node->childs[i], level + 1));
    }

    free (qn);  qn = NULL;
  }
  printf("\n");
  queue_delete (q); q = NULL;

  return;
}

void btree_print_2d (BTree *tree)
{
  if (! tree || ! tree->root)
    return;

  btree_print_2d_util (tree->root);
}