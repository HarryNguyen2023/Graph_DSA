#ifndef __B_PLUS_TREE_H__
#define __B_PLUS_TREE_H__

typedef struct BPlusTreeNode BPlusTreeNode;

struct BPlusTreeNode
{
  int             count;
  unsigned char   is_leaf;
  int             *key;
  BPlusTreeNode   **childs;
  BPlusTreeNode   *parent;
  void            **data;
  BPlusTreeNode   *next;
};

typedef struct BPlusTree
{
  int           size;
  int           t;
  BPlusTreeNode *root;
} BPlusTree;

BPlusTree* bp_tree_create (int t);
void bp_tree_delete (BPlusTree *tree);
BPlusTreeNode* bp_tree_search (BPlusTree *tree, int key, int *index);
BPlusTreeNode* bp_tree_insert (BPlusTree *tree, int key, void *data);
void bp_tree_print_2d (BPlusTree *tree);
int bp_tree_remove (BPlusTree *tree, int key);
void bp_tree_linked_test (BPlusTree *tree);

#endif /* __B_PLUS_TREE_H__ */