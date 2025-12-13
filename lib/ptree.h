#ifndef __PTREE_H__
#define __PTREE_H__

struct ptree_node;

struct ptree
{
  struct ptree_node *top;
  unsigned int max_key_length;
};

struct ptree_node
{
  struct ptree_node *link[2];
#define p_left    link[0]
#define p_right   link[1]

  void *info;
  struct ptree *tree;
  struct ptree_node *parent;
  unsigned int lock;
  unsigned int key_length;
  unsigned char key[1];
};

struct ptree *
ptree_init (unsigned int max_key_length);
void
ptree_free (struct ptree *tree);
void
ptree_unlock_node (struct ptree_node *node);
void
ptree_node_free (struct ptree_node *node);
struct ptree_node *
ptree_node_create (struct ptree *tree, unsigned char *key, unsigned int key_length);
struct ptree_node *
ptree_node_lookup (struct ptree *tree, unsigned char *key, unsigned int key_length);
struct ptree_node *
ptree_node_get (struct ptree *tree, unsigned char *key, unsigned int key_length);
void
ptree_print (struct ptree *tree);
void
ptree_dump (struct ptree *tree);
void
ptree_node_remove (struct ptree_node *node);

#endif /* __PTREE_H__ */