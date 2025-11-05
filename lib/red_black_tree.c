#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "binary_tree.h"
#include "red_black_tree.h"

RedBlackTreeNode *rb_tree_node_create (int key, void *data)
{
  RedBlackTreeNode *node = NULL;

  node = (RedBlackTreeNode *)malloc(sizeof(RedBlackTreeNode));
  if (! node)
  {
    printf ("[%s,%d] Fail to create Red Black Tree Node\n", __func__, __LINE__);
    return NULL;
  }

  node->left   = NULL;
  node->right  = NULL;
  node->info   = data;
  node->color  = RED_COLOR;
  node->key    = key;
  node->parent = NULL;

  return node;
}

void rb_tree_node_delete (RedBlackTreeNode *node)
{
  if (! node)
    return;

  free (node);
  node = NULL;
}

RedBlackTree *rb_tree_create (void)
{
  RedBlackTree *tree = (RedBlackTree *)malloc(sizeof (RedBlackTree));
  if (tree)
  {
    tree->size = 0;
    tree->root = NULL;
  }

  return tree;
}

void rb_tree_delete_util (RedBlackTreeNode *root)
{
  bin_tree_delete_util ((BinTreeNode *)root);
}

void rb_tree_delete (RedBlackTree *tree)
{
  bin_tree_delete ((BinTree *)tree);
}

void rb_tree_inorder_traversal (RedBlackTreeNode *root)
{
  bin_tree_inorder_traversal ((BinTreeNode *)root);
}

void rb_tree_preorder_traversal (RedBlackTreeNode *root)
{
  bin_tree_preorder_traversal ((BinTreeNode *)root);
}

void rb_tree_postorder_traversal (RedBlackTreeNode *root)
{
  bin_tree_postorder_traversal ((BinTreeNode *)root);
}

RedBlackTreeNode* rb_tree_rotate_left (RedBlackTreeNode *x)
{
  RedBlackTreeNode *y = NULL;

  if (! x)
    return NULL;

  y           = x->right;
  if (y)
  {
    if (y->left)
      y->left->parent = x;
    x->right        = y->left;
    y->left         = x;
    y->parent       = x->parent;
    x->parent       = y;
  }

  return y;
}

RedBlackTreeNode* rb_tree_rotate_right (RedBlackTreeNode *x)
{
  RedBlackTreeNode *y = NULL;

  if (! x)
    return NULL;

  y           = x->left;
  if (y)
  {
    if (y->right)
      y->right->parent = x;
    x->left          = y->right;
    y->right         = x;
    y->parent        = x->parent;
    x->parent        = y;
  }

  return y;
}

int rb_tree_is_leaf_node (RedBlackTreeNode *node)
{
  return bin_tree_is_leaf_node ((BinTreeNode *)node);
}

void rb_tree_print_util (RedBlackTreeNode *node, int count, int space)
{
  if (node == NULL)
      return;

  space += count;

  rb_tree_print_util (node->right, count, space);

  printf ("\n");
  for (int i = count; i < space; i++)
    printf (" ");
  printf ("%d-%c\n", node->key, (node->color == RED_COLOR) ? 'R' : 'B');

  rb_tree_print_util (node->left, count, space);    
}

void rb_tree_print (RedBlackTree *tree)
{
  if (! tree)
    return;

  printf ("Tree size: %d\n", tree->size);
  rb_tree_print_util (tree->root, 10, 0);
}

RedBlackTreeNode *rb_tree_node_search_util (RedBlackTreeNode *root, int key)
{
  if (! root)
    return NULL;

  if (key == root->key)
    return root;
  else if (key < root->key)
    return rb_tree_node_search_util (root->left, key);
  else
    return rb_tree_node_search_util (root->right, key);
}

RedBlackTreeNode *rb_tree_node_search (RedBlackTree *tree, int key)
{
  if (! tree || ! tree->size)
    return NULL;
  return rb_tree_node_search_util (tree->root, key);
}

void rb_tree_insert_balance_fix (RedBlackTreeNode **root, RedBlackTreeNode *node)
{
  RedBlackTreeNode *parent_node = NULL;

  if (! node)
    return;

  while (node->color == RED_COLOR
        && node->parent && node->parent->color == RED_COLOR)
  {
    if (node->parent->parent
        && node->parent->key < node->parent->parent->key)
    {
      if (node->parent->parent->right
          && node->parent->parent->right->color == RED_COLOR)
      {
        node->parent->parent->right->color  = BLACK_COLOR;
        if (node->parent->parent->left)
          node->parent->parent->left->color = BLACK_COLOR;
        node->parent->parent->color         = RED_COLOR;
        node = node->parent->parent;
      }
      else if (node->key > node->parent->key)
      {
        node = node->parent;
        parent_node = node->parent;
        if (parent_node)
        {
          if (node == parent_node->left)
            parent_node->left = rb_tree_rotate_left (node);
          else
            parent_node->right = rb_tree_rotate_left (node);
        }
        else
        {
          *root = rb_tree_rotate_left (node);
          break;
        }
      }
      else
      {
        node->parent->color = BLACK_COLOR;
        if (node->parent->parent)
        {
          node->parent->parent->color = RED_COLOR;
          parent_node = node->parent->parent->parent;
          if (parent_node)
          {
            if (node->parent->parent == parent_node->left)
              parent_node->left = rb_tree_rotate_right (node->parent->parent);
            else
              parent_node->right = rb_tree_rotate_right (node->parent->parent);
          }
          else
          {
            *root = rb_tree_rotate_right (node->parent->parent);
            break;
          }
        }
      }
    }
    else if (node->parent->parent)
    {
      if (node->parent->parent->left
          && node->parent->parent->left->color == RED_COLOR)
      {
        node->parent->parent->left->color     = BLACK_COLOR;
        if (node->parent->parent->right)
          node->parent->parent->right->color  = BLACK_COLOR;
        node->parent->parent->color           = RED_COLOR;
        node = node->parent->parent;
      }
      else if (node->key < node->parent->key)
      {
        node = node->parent;
        parent_node = node->parent;
        if (parent_node)
        {
          if (node == parent_node->left)
            parent_node->left = rb_tree_rotate_right (node);
          else
            parent_node->right = rb_tree_rotate_right (node);
        }
        else
        {
          *root = rb_tree_rotate_right (node);
          break;
        }
      }
      else
      {
        node->parent->color = BLACK_COLOR;
        if (node->parent->parent)
        {
          node->parent->parent->color = RED_COLOR;
          parent_node = node->parent->parent->parent;
          if (parent_node)
          {
            if (node->parent->parent == parent_node->left)
              parent_node->left = rb_tree_rotate_left (node->parent->parent);
            else
              parent_node->right = rb_tree_rotate_left (node->parent->parent);
          }
          else
          {
            *root = rb_tree_rotate_left (node->parent->parent);
            break;
          }
        }
      }
    }
    else
      break;
  }
}

RedBlackTreeNode* rb_tree_insert_util (RedBlackTreeNode **root, int key, void *data)
{
  RedBlackTreeNode* temp = NULL, *new_node = NULL;

  if (! *root)
    return rb_tree_node_create (key, data);

  temp = *root;
  while (temp)
  {
    if (key < temp->key)
    {
      if (temp->left)
        temp = temp->left;
      else
        break;
    }
    else if (key > temp->key)
    {
      if (temp->right)
        temp = temp->right;
      else
        break;
    }
    else
      return temp;
  }

  new_node = rb_tree_node_create (key, data);
  if (! new_node)
  {
    printf ("[%s,%d] Fail to create new RB Tree node\n", __func__, __LINE__);
    return NULL;
  }

  new_node->parent = temp;
  if (key < temp->key)
    temp->left = new_node;
  else
    temp->right = new_node;

  if (! temp->parent)
    return *root;

  rb_tree_insert_balance_fix (root, new_node);

  return *root;
}

RedBlackTreeNode* rb_tree_insert (RedBlackTree *tree, int key, void *data)
{
  RedBlackTreeNode* new_node = NULL;

  if (! tree)
    return NULL;

  new_node = rb_tree_insert_util (&(tree->root), key, data);
  if (new_node)
  {
    tree->size++;
    if (tree->root == NULL)
      tree->root      = new_node;
    tree->root->color = BLACK_COLOR;
  }
  return new_node;
}