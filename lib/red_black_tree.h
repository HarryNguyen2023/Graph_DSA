#ifndef __RED_BLACK_TREE_H__
#define __RED_BLACK_TREE_H__

#define BLACK_COLOR (1)
#define RED_COLOR   (2)

typedef struct RedBlackTreeNode RedBlackTreeNode;

struct RedBlackTreeNode
{
  RedBlackTreeNode *left;
  RedBlackTreeNode *right;
  void             *info;
  int               key;
  unsigned char     color;
  RedBlackTreeNode *parent;
};

typedef struct RedBlackTree
{
  int               size;
  RedBlackTreeNode *root;
} RedBlackTree;

RedBlackTree *rb_tree_create (void);
void rb_tree_delete (RedBlackTree *tree);
void rb_tree_inorder_traversal (RedBlackTreeNode *root);
void rb_tree_preorder_traversal (RedBlackTreeNode *root);
void rb_tree_postorder_traversal (RedBlackTreeNode *root);
void rb_tree_print (RedBlackTree *tree);
RedBlackTreeNode *rb_tree_node_search (RedBlackTree *tree, int key);
RedBlackTreeNode* rb_tree_insert (RedBlackTree *tree, int key, void *data);
void rb_tree_remove (RedBlackTree *tree, int key);

#endif /* __RED_BLACK_TREE_H__ */