#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "binary_tree.h"
#include "avl_tree.h"

#define MAX(A,B)  (((A) > (B)) ? A : B)

AvlTreeNode *avl_tree_node_create (int key, void *data)
{
  AvlTreeNode *node = NULL;

  node = (AvlTreeNode *)malloc(sizeof(AvlTreeNode));
  if (! node)
  {
    printf ("[%s,%d] Fail to create AVL Tree Node\n", __func__, __LINE__);
    return NULL;
  }

  node->left   = NULL;
  node->right  = NULL;
  node->info   = data;
  node->height = 1;
  node->key    = key;

  return node;
}

void avl_tree_node_delete (AvlTreeNode *node)
{
  if (! node)
    return;

  free (node);
  node = NULL;
}

AvlTree *avl_tree_create (void)
{
  AvlTree *tree = (AvlTree *)malloc(sizeof (AvlTree));
  if (tree)
  {
    tree->size = 0;
    tree->root = NULL;
  }

  return tree;
}

void avl_tree_delete_util (AvlTreeNode *root)
{
  bin_tree_delete_util ((BinTreeNode *)root);
}

void avl_tree_delete (AvlTree *tree)
{
  bin_tree_delete ((BinTree *)tree);
}

void avl_tree_inorder_traversal (AvlTreeNode *root)
{
  bin_tree_inorder_traversal ((BinTreeNode *)root);
}

void avl_tree_preorder_traversal (AvlTreeNode *root)
{
  bin_tree_preorder_traversal ((BinTreeNode *)root);
}

void avl_tree_postorder_traversal (AvlTreeNode *root)
{
  bin_tree_postorder_traversal ((BinTreeNode *)root);
}

int avl_tree_get_height (AvlTreeNode *node)
{
  if (! node)
    return 0;

  return (node->height);
}

AvlTreeNode* avl_tree_rotate_left (AvlTreeNode *x)
{
  AvlTreeNode *y = NULL;

  if (! x)
    return NULL;

  y           = x->right;
  if (y)
  {
    x->right  = y->left;
    y->left   = x;
  }

  if (x)
    x->height = MAX (avl_tree_get_height (x->left), avl_tree_get_height(x->right)) + 1;
  if (y)
    y->height = MAX (avl_tree_get_height (y->left), avl_tree_get_height(y->right)) + 1;
  return y;
}

AvlTreeNode* avl_tree_rotate_right (AvlTreeNode *x)
{
  AvlTreeNode *y = NULL;

  if (! x)
    return NULL;

  y           = x->left;
  if (y)
  {
    x->left   = y->right;
    y->right  = x;
  }

  if (x)
    x->height = MAX (avl_tree_get_height (x->left), avl_tree_get_height(x->right)) + 1;
  if (y)
    y->height = MAX (avl_tree_get_height (y->left), avl_tree_get_height(y->right)) + 1;
  return y;
}

AvlTreeNode *avl_tree_node_search_util (AvlTreeNode *root, int key)
{
  if (! root)
    return NULL;

  if (key == root->key)
    return root;
  else if (key < root->key)
    return avl_tree_node_search_util (root->left, key);
  else
    return avl_tree_node_search_util (root->right, key);
}

AvlTreeNode *avl_tree_node_search (AvlTree *tree, int key)
{
  if (! tree || ! tree->size)
    return NULL;
  return avl_tree_node_search_util (tree->root, key);
}

int avl_tree_get_balance (AvlTreeNode *root)
{
  if (! root)
    return 0;

  return (avl_tree_get_height (root->left) - avl_tree_get_height(root->right));
}

AvlTreeNode* avl_tree_insert_util (AvlTreeNode *root, int key, void *data)
{
  int balance = 0;

  if (! root)
    return avl_tree_node_create (key, data);

  if (key < root->key)
  {
    root->left = avl_tree_insert_util (root->left, key, data);
  }
  else if (key > root->key)
  {
    root->right = avl_tree_insert_util (root->right, key, data);
  }
  else
    return root;

  /* Update node height */
  root->height = MAX(avl_tree_get_height (root->left), avl_tree_get_height(root->right)) + 1;
  balance      = avl_tree_get_balance (root);

  /* 
   *       p
   *      /
   *    x
   *   /
   *  y
   */
  if (balance > 1
      && root->left && key < root->left->key)
    return avl_tree_rotate_right (root);

  /* 
   * p
   *  \
   *   x
   *    \
   *     y
   */
  if (balance < -1
      && root->right && key > root->right->key)
    return avl_tree_rotate_left (root);

  /* 
   *       p
   *      /
   *    x
   *     \
   *      y
   */
  if (balance > 1
      && root->left && key > root->left->key)
  {
    root->left = avl_tree_rotate_left (root->left);
    return avl_tree_rotate_right (root);
  }

  /* 
   *   p
   *    \
   *     x
   *    /
   *   y
   */
  if (balance < -1
      && root->right && key < root->right->key)
  {
    root->right = avl_tree_rotate_right (root->right);
    return avl_tree_rotate_left (root);
  }

  return root;
}

AvlTreeNode* avl_tree_insert (AvlTree *tree, int key, void *data)
{
  AvlTreeNode* new_node = NULL;

  if (! tree)
    return NULL;

  if (key == 79)
    printf ("\n");

  new_node = avl_tree_insert_util (tree->root, key, data);
  if (new_node)
  {
    tree->size++;
    if (tree->root == NULL)
      tree->root = new_node;
  }
  return new_node;
}

void avl_tree_print_util (AvlTreeNode *node, int count, int space)
{
  if (node == NULL)
      return;

  space += count;

  avl_tree_print_util (node->right, count, space);

  printf ("\n");
  for (int i = count; i < space; i++)
    printf (" ");
  printf ("%d\n", node->key);

  avl_tree_print_util (node->left, count, space);    
}

void avl_tree_print (AvlTree *tree)
{
  if (! tree)
    return;

  printf ("Tree size: %d\n", tree->size);
  avl_tree_print_util (tree->root, 15, 0);
}

AvlTreeNode* avl_tree_get_min_successor (AvlTreeNode *node)
{
  if (! node)
    return NULL;

  while (node->left)
    node = node->left;
  return node;
}

int avl_tree_is_leaf_node (AvlTreeNode *node)
{
  return bin_tree_is_leaf_node ((BinTreeNode *)node);
}

int avl_tree_is_node_has_one_child (AvlTreeNode *node)
{
  return (bin_tree_is_leaf_node ((BinTreeNode *)node)) ? \
            0 : ((node->left == NULL || node->right == NULL) ? 1 : 0);
}

void avl_tree_node_swap (AvlTreeNode *node_1, AvlTreeNode *node_2)
{
  int key;
  void *data;

  if (! node_1 || ! node_2)
    return;

  key           = node_1->key;
  data          = node_1->info;
  node_1->key   = node_2->key;
  node_1->info  = node_2->info;
  node_2->key   = key;
  node_2->info  = data;
}

AvlTreeNode* avl_tree_remove_util (AvlTreeNode *root, int key)
{
  int balance = 0;
  AvlTreeNode *temp = NULL;

  if (key < root->key)
  {
    root->left = avl_tree_remove_util (root->left, key);
  }
  else if (key > root->key)
  {
    root->right = avl_tree_remove_util (root->right, key);
  }
  else
  {
    if (avl_tree_is_leaf_node (root))
    {
      free (root);
      root = NULL;
    }
    else if (avl_tree_is_node_has_one_child (root))
    {
      temp = (root->left) ? root->left : root->right;
      if (temp)
      {
        *root = *temp;
        free (temp);
        temp = NULL;
      }
    }
    else
    {
      temp = avl_tree_get_min_successor (root->right);
      if (temp)
      {
        avl_tree_node_swap (root, temp);
        root->right = avl_tree_remove_util (root->right, temp->key);
      }
    }
  }

  if (! root)
    return root;

  /* Update node height */
  root->height = MAX(avl_tree_get_height (root->left), avl_tree_get_height(root->right)) + 1;
  balance      = avl_tree_get_balance (root);

  /* 
   *       p
   *      /
   *    x
   *   /
   *  y
   */
  if (balance > 1
      && avl_tree_get_balance (root->left) >= 0)
    return avl_tree_rotate_right (root);

  /* 
   * p
   *  \
   *   x
   *    \
   *     y
   */
  if (balance < -1
      && avl_tree_get_balance (root->right) <= 0)
    return avl_tree_rotate_left (root);

  /* 
   *       p
   *      /
   *    x
   *     \
   *      y
   */
  if (balance > 1
      && avl_tree_get_balance (root->left) < 0)
  {
    root->left = avl_tree_rotate_left (root->left);
    return avl_tree_rotate_right (root);
  }

  /* 
   *   p
   *    \
   *     x
   *    /
   *   y
   */
  if (balance < -1
      && avl_tree_get_balance (root->right) > 0)
  {
    root->right = avl_tree_rotate_right (root->right);
    return avl_tree_rotate_left (root);
  }

  return root;
}

AvlTreeNode* avl_tree_remove (AvlTree *tree, int key)
{
  AvlTreeNode* node = NULL;

  if (! tree)
    return NULL;

  node = avl_tree_remove_util (tree->root, key);
  if (node)
    tree->size--;
  return node;
}