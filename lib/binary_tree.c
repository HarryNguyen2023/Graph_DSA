#include <stdio.h>
#include <stdlib.h>
#include "binary_tree.h"

BinTreeNode *bin_tree_node_create (void *data, void (*bin_tree_node_dump)(void *), void (*bin_tree_node_del)(void *))
{
  BinTreeNode *node = NULL;

  node = (BinTreeNode *)malloc(sizeof(BinTreeNode));
  if (! node)
  {
    printf ("[%s,%d] Fail to create Binary Tree Node\n", __func__, __LINE__);
    return NULL;
  }

  node->left  = NULL;
  node->right = NULL;
  node->data  = data;
  node->bin_tree_node_dump  = bin_tree_node_dump;
  node->bin_tree_node_del   = bin_tree_node_del;
}

void bin_tree_node_delete (BinTreeNode *node)
{
  if (! node)
    return;

  free (node);
  node = NULL;
}

BinTree *bin_tree_create (void)
{
  BinTree *tree = NULL;

  tree = (BinTree *)malloc(sizeof(BinTree));
  if (! tree)
  {
    printf ("[%s,%d] Fail to create Binary Tree\n", __func__, __LINE__);
    return NULL;
  }

  tree->size = 0;
  tree->root = NULL;
}

void bin_tree_delete_util (BinTreeNode *root)
{
  if (! root)
    return;

  if (root->left)
    bin_tree_delete_util (root->left);

  if (root->right)
    bin_tree_delete_util (root->right);

  bin_tree_node_delete (root);
}

void bin_tree_delete (BinTree *tree)
{
  if (! tree)
    return;

  bin_tree_delete_util (tree->root);
}

BinTreeNode *bin_tree_insert_left (BinTreeNode *root, void *data)
{
  if (! root)
    return NULL;

  root->left = bin_tree_node_create (data, root->bin_tree_node_dump, root->bin_tree_node_del);
  return root->left;
}

BinTreeNode *bin_tree_insert_right (BinTreeNode *root, void *data)
{
  if (! root)
    return NULL;

  root->right = bin_tree_node_create (data, root->bin_tree_node_dump, root->bin_tree_node_del);
  return root->right;
}

void bin_tree_inorder_traversal (BinTreeNode *root)
{
  if (! root)
    return;

  if (root->left)
    bin_tree_inorder_traversal (root->left);

  if (root->bin_tree_node_dump)
    (*root->bin_tree_node_dump)(root->data);

  if (root->right)
    bin_tree_inorder_traversal (root->right);
}

void bin_tree_preorder_traversal (BinTreeNode *root)
{
  if (! root)
    return;

  if (root->bin_tree_node_dump)
    (*root->bin_tree_node_dump)(root->data);

  if (root->left)
    bin_tree_preorder_traversal (root->left);

  if (root->right)
    bin_tree_preorder_traversal (root->right);
}

void bin_tree_postorder_traversal (BinTreeNode *root)
{
  if (! root)
    return;

  if (root->left)
    bin_tree_postorder_traversal (root->left);

  if (root->right)
    bin_tree_postorder_traversal (root->right);

  if (root->bin_tree_node_dump)
    (*root->bin_tree_node_dump)(root->data);
}

int bin_tree_is_leaf_node (BinTreeNode *node)
{
  if (! node)
    return 0;

  return (node->left == NULL && node->right == NULL);
}