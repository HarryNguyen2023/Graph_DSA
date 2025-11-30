#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"
#include "b_tree.h"
#include "b_plus_tree.h"

BPlusTreeNode* bp_tree_remove_until (BPlusTreeNode **root, BPlusTreeNode *node, int t, int key);

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
      if (node->is_leaf)
      {
        if (node->next)
          sib->next = node->next;
        node->next = sib;
      }
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

void bp_tree_remove_node (BPlusTreeNode *node, int index)
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

BPlusTreeNode* bp_tree_get_inorder_pred (BPlusTreeNode *node, int idx)
{
  BPlusTreeNode* pred = NULL;

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

BPlusTreeNode* bp_tree_get_inorder_succ (BPlusTreeNode *node, int idx)
{
  BPlusTreeNode* succ = NULL;

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

int bp_tree_borrow_from_left_sib (BPlusTreeNode *node, int t)
{
  BPlusTreeNode *sib = NULL, *parent = NULL;
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
  bp_tree_insert_node (node, sib->key[sib->count - 1], sib->data[sib->count - 1], NULL);

  /* Put the rightmost key of the left sibling to the parent */
  bp_tree_insert_node (parent, sib->key[sib->count - 1], NULL, NULL);
  bp_tree_remove_node (sib, sib->count - 1);

  bp_tree_remove_node (parent, i);

  return 0;
}

int bp_tree_borrow_from_right_sib (BPlusTreeNode *node, int t)
{
  BPlusTreeNode *sib = NULL, *parent = NULL;
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
  bp_tree_insert_node (node, sib->key[0], sib->data[0], NULL);

  /* Put the leftmost key of the right sibling to the parent */
  bp_tree_insert_node (parent, sib->key[1], NULL, NULL);
  bp_tree_remove_node (sib, 0);

  bp_tree_remove_node (parent, i);

  return 0;
}

BPlusTreeNode* bp_tree_merge_node (BPlusTreeNode* left, BPlusTreeNode *right)
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

  bp_tree_node_delete (right);
  return left;
}

void bp_tree_node_fill (BPlusTreeNode **root, BPlusTreeNode *node)
{
  int i, parent_index;
  BPlusTreeNode *parent = NULL, *sib = NULL, *temp = NULL;

  parent = node->parent;
  if (! parent)
    return;

  if (node == parent->childs[0])
  {
    parent_index = 0;
    sib = parent->childs[1];
    for (i = 0; i <= sib->count; ++i)
    {
      node->childs[i + node->count + 1] = sib->childs[i];
      if (sib->childs[i])
        sib->childs[i]->parent = node;
    }
    temp = bp_tree_merge_node (node, sib);
  }
  else
  {
    while (i < parent->count)
    {
      if (parent->childs[i] == node)
        break;
      i++;
    }

    if (i > parent->count)
      return;

    sib = parent->childs[i - 1];
    parent_index = i - 1;

    for (i = 0; i <= node->count; ++i)
    {
      sib->childs[i + sib->count + 1] = node->childs[i];
      if (node->childs[i])
        node->childs[i]->parent = sib;
    }
    temp = bp_tree_merge_node (sib, node);
  }

  if (parent == *root
      && parent->count > 1)
  {
    bp_tree_insert_node (temp, parent->key[parent_index], NULL, NULL);
    bp_tree_remove_node (parent, parent_index);
    parent->childs[parent_index] = temp;
    for (i = parent_index + 1; i <= parent->count; ++i)
      parent->childs[i] = parent->childs[i + 1];
  }
  else
  {
    for (i = 0; i < parent->count; ++i)
      bp_tree_insert_node (temp, parent->key[i], NULL, NULL);

    *root = temp;
    bp_tree_node_delete (parent);
  }
}

void bp_tree_merge_from_parent (BPlusTreeNode **root, int t, BPlusTreeNode *node)
{
  int i = 0, j, key;
  BPlusTreeNode *temp = NULL, *parent = NULL;

  if (! node || ! (parent = node->parent))
    return;

  if (node == parent->childs[0])
  {
    bp_tree_remove_node (parent, 0);
    temp = bp_tree_merge_node (node, parent->childs[1]);
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

    if (i > parent->count)
      return;

    bp_tree_remove_node (parent, i - 1);

    temp = bp_tree_merge_node (parent->childs[i - 1], node);
    temp->parent->childs[i - 1] = temp;
    for (j = i; j <= parent->count; ++j)
      parent->childs[j] = parent->childs[j + 1];
  }

  if (parent->count < t)
  {
    bp_tree_node_fill (root, parent);
  }

  return;
}

void bp_tree_remove_from_leaf (BPlusTreeNode **root, BPlusTreeNode *node, int t, int index)
{
  int ret = 0;

  if (! node || ! node->is_leaf
      || index < 0 || index >= node->count)
    return;

  bp_tree_remove_node (node, index);
  if (node->count < t)
  {
    ret = bp_tree_borrow_from_left_sib (node, t);
    if (ret != 0)
    {
      ret = bp_tree_borrow_from_right_sib (node, t);
      if (ret != 0)
        bp_tree_merge_from_parent (root, t, node);
    }
  }

  return;
}

void bp_tree_remove_from_non_leaf (BPlusTreeNode **root, BPlusTreeNode *node, int t, int index)
{
  int i, ret;
  BPlusTreeNode *temp = NULL, *sib = NULL, *parent = NULL;

  if (! node || node->is_leaf
      || index < 0 || index >= node->count)
    return;

  if (node->childs[index] && node->childs[index]->count > t)
  {
    temp = bp_tree_get_inorder_pred (node, index);
    if (temp)
    {
      bp_tree_remove_node (node, index);
      bp_tree_insert_node (node, temp->key[temp->count - 1], NULL, NULL);
      sib = bp_tree_get_inorder_succ (node, index);
      if (sib)
      {
        bp_tree_remove_node (sib, 0);
        bp_tree_insert_node (sib, temp->key[temp->count - 1], temp->data[temp->count - 1], NULL);
      }
      bp_tree_remove_until (root, node->childs[index], t, temp->key[temp->count - 1]);
    }
  }
  else if (node->childs[index + 1] && node->childs[index + 1]->count > t)
  {
    temp = bp_tree_get_inorder_succ (node, index);
    if (temp)
    {
      bp_tree_remove_node (node, index);
      bp_tree_insert_node (node, temp->key[1], NULL, NULL);
      bp_tree_remove_until (root, node->childs[index + 1], t, temp->key[0]);
    }
  }
  else
  {
    temp = bp_tree_merge_node (node->childs[index], node->childs[index + 1]);
    if (temp)
    {
      for (i = index + 1; i <= node->count - 1; ++i)
        node->childs[i] = node->childs[i + 1];
    }
    bp_tree_remove_until (root, temp, t, node->key[index]);
    bp_tree_remove_node (node, index);
    if (node->count < t)
    {
      bp_tree_node_fill (root, node);
    }
  }

  return;
}

BPlusTreeNode* bp_tree_remove_until (BPlusTreeNode **root, BPlusTreeNode *node, int t, int key)
{
  BPlusTreeNode* del_node = NULL;
  int del_key_index = -1;
  int i = 0;

  if (! node)
    return NULL;

  while (i < node->count && key > node->key[i])
    i++;

  if (key == node->key[i])
  {
    if (node->is_leaf)
      bp_tree_remove_from_leaf (root, node, t, i);
    else
      bp_tree_remove_from_non_leaf (root, node, t, i);
    return node;
  }

  return bp_tree_remove_until (root, node->childs[i], t, key);
}

int bp_tree_remove (BPlusTree *tree, int key)
{
  if (! tree || ! tree->root)
    return -1;

  if (bp_tree_remove_until (&(tree->root), tree->root, tree->t, key))
    tree->size--;

  return 0;
}

void bp_tree_linked_test (BPlusTree *tree)
{
  BPlusTreeNode *node = NULL;
  int i = 0;

  if (! tree || ! tree->root || ! tree->size)
    return;

  node = tree->root;
  /* Get the left most node */
  while (node && node->childs[0])
    node = node->childs[0];

  do
  {
    printf ("(%d", node->key[0]);
    for (i = 1; i < node->count; ++i)
      printf (",%d", node->key[i]);
    printf (") -> ");
    node = node->next;
  } while (node);

  return;
}