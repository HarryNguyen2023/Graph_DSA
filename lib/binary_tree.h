#ifndef __BINARY_TREE_H__
#define __BINARY_TREE_H__

typedef struct BinTreeNode BinTreeNode;

struct BinTreeNode
{
  BinTreeNode *left;
  BinTreeNode *right;
  void        *data;
  void        (*bin_tree_node_dump) (void *);
};

typedef struct BinTree
{
  int size;
  BinTreeNode *root;
} BinTree;

BinTreeNode *bin_tree_node_create (void *data, void (*bin_tree_node_dump)(void *));
void bin_tree_node_delete (BinTreeNode *node);
BinTree *bin_tree_create (void);
void bin_tree_delete_util (BinTreeNode *root);
void bin_tree_delete (BinTree *tree);
BinTreeNode *bin_tree_insert_left (BinTreeNode *root, void *data);
BinTreeNode *bin_tree_insert_right (BinTreeNode *root, void *data);
void bin_tree_inorder_traversal (BinTreeNode *root);
void bin_tree_preorder_traversal (BinTreeNode *root);
void bin_tree_postorder_traversal (BinTreeNode *root);
int bin_tree_is_leaf_node (BinTreeNode *node);

#endif /* __BINARY_TREE_H__ */