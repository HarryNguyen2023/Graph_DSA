#ifndef __AVL_TREE_H__
#define __AVL_TREE_H__

typedef struct AvlTreeNode AvlTreeNode;

struct AvlTreeNode
{
  AvlTreeNode *left;
  AvlTreeNode *right;
  void        *info;
  int         key;
  int         height;
};

typedef struct AvlTree
{
  int         size;
  AvlTreeNode *root;
} AvlTree;

AvlTree *avl_tree_create (void);
void avl_tree_delete (AvlTree *tree);
void avl_tree_inorder_traversal (AvlTreeNode *root);
void avl_tree_preorder_traversal (AvlTreeNode *root);
void avl_tree_postorder_traversal (AvlTreeNode *root);
AvlTreeNode *avl_tree_node_search (AvlTree *tree, int key);
AvlTreeNode* avl_tree_insert (AvlTree *tree, int key, void *data);
void avl_tree_print (AvlTree *tree);
AvlTreeNode* avl_tree_remove (AvlTree *tree, int key);

#endif /* __AVL_TREE_H__ */