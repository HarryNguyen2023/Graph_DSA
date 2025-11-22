#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"
#include "b_tree.h"

BTreeNode* b_tree_remove_until (BTreeNode **root, BTreeNode *node, int t, int key);

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

void b_tree_node_delete (BTreeNode *node)
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
    return b_tree_node_delete (root);

  for (i = 0; i < root->count + 1; ++i)
  {
    b_tree_delete_util (root->childs[i]);
  }

  b_tree_node_delete (root);
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
  node->key[t] = 0;
  node->data[t] = NULL;
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

void b_tree_print_2d_util (BTreeNode *root)
{
  Queue *q = NULL;
  BTreeQueueNode *qn = NULL;
  BTreeNode *node = NULL;
  int level = 0, cur_level = 0, height = 0;;

  if (! root)
  { printf("B Tree is empty\n");
    return;
  }

  /* Compute the height of the B Tree */
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

  /* Print the tree level by level */
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

    /* Indent according to height */
    int indent = (height - level) * 10;
    printf("%*s", indent, "");

    for (int i = 0; i < node->count; ++i) {
      printf("%d ", node->key[i]);
    }
    printf("  ");

    /* Enqueue children */
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

void b_tree_print_2d (BTree *tree)
{
  if (! tree || ! tree->root)
    return;

  b_tree_print_2d_util (tree->root);
}

BTreeNode* b_tree_get_inorder_succ (BTreeNode *node, int idx)
{
  BTreeNode* succ = NULL;

  if (! node)
    return NULL;

  if (idx < 0 && idx >= node->count)
  {
    printf ("[%s,%d] Invalid index range %d\n", __func__, __LINE__, idx);
    return NULL;
  }

  succ = node->childs[idx + 1];
  while (! succ->is_leaf)
    succ = succ->childs[0];

  return succ;
}

BTreeNode* b_tree_get_inorder_pred (BTreeNode *node, int idx)
{
  BTreeNode* pred = NULL;

  if (! node)
    return NULL;

  if (idx < 0 && idx >= node->count)
  {
    printf ("[%s,%d] Invalid index range %d\n", __func__, __LINE__, idx);
    return NULL;
  }

  pred = node->childs[idx];
  while (! pred->is_leaf)
    pred = pred->childs[pred->count];

  return pred;
}

void b_tree_remove_node (BTreeNode *node, int index)
{
  int i;

  if (! node || index < 0 || index >= node->count)
    return;

  for (i = index; i < node->count - 1; ++i)
  {
    node->key[i] = node->key[i + 1];
    node->data[i] = node->data[i + 1];
  }

  node->key[node->count - 1] = 0;
  node->data[node->count - 1] = NULL;
  node->count--;
  return;
}

int b_tree_borrow_from_left_sib (BTreeNode *node, int t)
{
  BTreeNode *sib = NULL, *parent = NULL;
  int i = 0, ret = 0;

  parent = node->parent;
  if (! parent)
    return -1;

  while (i <= parent->count)
  {
    if (parent->childs[i] == node)
      break;
    i++;
  }

  if (i == 0 || i > parent->count)
    return -1;

  sib = parent->childs[i - 1];
  if (sib->count <= t)
    return -1;

  /* Move the parent key down to the current node */
  b_tree_insert_node (node, parent->key[i - 1], parent->data[i - 1], NULL);

  /* Put the rightmost key of the left sibling to the parent */
  b_tree_insert_node (parent, sib->key[sib->count - 1], sib->data[sib->count - 1], NULL);
  b_tree_remove_node (sib, sib->count - 1);

  b_tree_remove_node (parent, i);

  return 0;
}

int b_tree_borrow_from_right_sib (BTreeNode *node, int t)
{
  BTreeNode *sib = NULL, *parent = NULL;
  int i = 0, ret = 0;

  parent = node->parent;
  if (! parent)
    return -1;

  while (i < parent->count)
  {
    if (parent->childs[i] == node)
      break;
    i++;
  }

  if (i >= parent->count)
    return -1;

  sib = parent->childs[i + 1];
  if (sib->count <= t)
    return -1;

  /* Move the parent key down to the current node */
  b_tree_insert_node (node, parent->key[i], parent->data[i], NULL);

  /* Put the leftmost key of the right sibling to the parent */
  b_tree_insert_node (parent, sib->key[0], sib->data[0], NULL);
  b_tree_remove_node (sib, 0);

  b_tree_remove_node (parent, i);

  return 0;
}

BTreeNode* b_tree_merge_node (BTreeNode* left, BTreeNode *right)
{
  int i = 0;

  if (! left || ! right)
    return NULL;

  for (i = 0; i < right->count; ++i)
  {
    left->key[i + left->count] = right->key[i];
    left->data[i + left->count] = right->data[i];
  }
  left->count += right->count;

  b_tree_node_delete (right);
  return left;
}

void b_tree_node_fill (BTreeNode **root, BTreeNode *node)
{
  int i;
  BTreeNode *parent = NULL, *sib = NULL, *temp = NULL;

  parent = node->parent;
  if (! parent)
    return;

  sib = (node == parent->childs[0]) ?  \
          parent->childs[1] : parent->childs[0];

  if (node == parent->childs[0])
  {
    for (i = 0; i <= sib->count; ++i)
    {
      node->childs[i + node->count + 1] = sib->childs[i];
      if (sib->childs[i])
        sib->childs[i]->parent = node;
    }
    temp = b_tree_merge_node (node, sib);
  }
  else
  {
    for (i = 0; i <= node->count; ++i)
    {
      sib->childs[i + sib->count + 1] = node->childs[i];
      if (node->childs[i])
        node->childs[i]->parent = sib;
    }
    temp = b_tree_merge_node (sib, node);
  }

  for (i = 0; i < parent->count; ++i)
    b_tree_insert_node (temp, parent->key[i], parent->data[i], NULL);

  *root = temp;
}

void b_tree_merge_from_parent (BTreeNode **root, int t, BTreeNode *node)
{
  int i = 0, j;
  BTreeNode *temp = NULL, *parent = NULL;

  if (! node || ! (parent = node->parent))
    return;

  /* Case deleted node is in the first child */
  if (node == parent->childs[0])
  {
    b_tree_insert_node (node, parent->key[0], parent->data[0], NULL);

    b_tree_remove_node (parent, 0);

    temp = b_tree_merge_node (node, parent->childs[1]);
    parent->childs[0] = temp;
    for (i = 1; i <= parent->count; ++i)
      parent->childs[i] = parent->childs[i + 1];
  }
  else
  {
    while (i < parent->count)
    {
      if (parent->childs[i] == node)
        break;
      i++;
    }

    if (i >= parent->count)
      return;

    b_tree_insert_node (node, parent->key[i - 1], parent->data[i - 1], NULL);

    b_tree_remove_node (parent, i - 1);

    temp = b_tree_merge_node (parent->childs[i - 1], node);
    parent->childs[i - 1] = temp;
    for (j = i; j <= parent->count; ++j)
      parent->childs[j] = parent->childs[j + 1];
  }

  if (parent->count < t)
  {
    b_tree_node_fill (root, parent);
  }

  return;
}

void b_tree_remove_from_leaf (BTreeNode **root, BTreeNode *node, int t, int index)
{
  int ret = 0;

  if (! node || ! node->is_leaf
      || index < 0 || index >= node->count)
    return;

  b_tree_remove_node (node, index);
  if (node->count < t)
  {
    ret = b_tree_borrow_from_left_sib (node, t);
    if (ret != 0)
    {
      ret = b_tree_borrow_from_right_sib (node, t);
      if (ret != 0)
      {
        b_tree_merge_from_parent (root, t, node);
      }
    }
  }

  return;
}

void b_tree_remove_from_non_leaf (BTreeNode **root, BTreeNode *node, int t, int index)
{
  int i;
  BTreeNode *temp = NULL, *sib = NULL, *parent = NULL;

  if (! node || node->is_leaf
      || index < 0 || index >= node->count)
    return;

  if (node->childs[index] && node->childs[index]->count > t)
  {
    temp = b_tree_get_inorder_pred (node, index);
    if (temp)
    {
      b_tree_remove_node (node, index);
      b_tree_insert_node (node, temp->key[temp->count - 1], temp->data[temp->count - 1], NULL);
      b_tree_remove_until (root, node->childs[index], t, temp->key[temp->count - 1]);
    }
  }
  else if (node->childs[index + 1] && node->childs[index + 1]->count > t)
  {
    temp = b_tree_get_inorder_succ (node, index);
    if (temp)
    {
      b_tree_remove_node (node, index);
      b_tree_insert_node (node, temp->key[0], temp->data[0], NULL);
      b_tree_remove_until (root, node->childs[index + 1], t, temp->key[0]);
    }
  }
  else
  {
    temp = b_tree_merge_node (node->childs[index], node->childs[index + 1]);
    if (temp)
    {
      for (i = index + 1; i <= node->count - 1; ++i)
        node->childs[i] = node->childs[i + 1];
    }
    b_tree_remove_node (node, index);
    if (node->count < t)
    {
      b_tree_node_fill (root, node);
    }
  }

  return;
}

BTreeNode* b_tree_remove_until (BTreeNode **root, BTreeNode *node, int t, int key)
{
  BTreeNode* del_node = NULL;
  int del_key_index = -1;

  if (! node)
    return NULL;

  del_node = b_tree_search_util (node, key, &del_key_index);
  if (! del_node || del_key_index < 0)
  {
    printf ("Key %d does not exists in B Tree\n", key);
    return NULL;
  }

  if (del_node->is_leaf)
    b_tree_remove_from_leaf (root, del_node, t, del_key_index);
  else
    b_tree_remove_from_non_leaf (root, del_node, t, del_key_index);

  return node;
}

int b_tree_remove (BTree *tree, int key)
{
  if (! tree || ! tree->root)
    return -1;

  if (b_tree_remove_until (&(tree->root), tree->root, tree->t, key))
    tree->size--;

  return 0;
}