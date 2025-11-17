#ifndef __B_TREE_H__
#define __B_TREE_H__

typedef struct BTreeNode BTreeNode;

struct BTreeNode
{
  int           count;
  unsigned char is_leaf;
  int           *key;
  BTreeNode     **childs;
  BTreeNode     *parent;
  void          **data;
};

typedef struct BTree
{
  int size;
  int t;
  BTreeNode *root;
} BTree;

typedef struct BTreeQueueNode
{
  int       level;
  BTreeNode *node;
} BTreeQueueNode;

BTree* b_tree_create (int t);
void b_tree_delete (BTree *tree);
BTreeNode* b_tree_search (BTree *tree, int key, int *index);
BTreeNode* b_tree_insert (BTree *tree, int key, void *data);
void b_tree_print_2d (BTree *tree);
int b_tree_remove (BTree *tree, int key);

#endif /* __B_TREE_H__ */