#ifndef __HASH_TABLE_H__
#define __HASH_TABLE_H__

typedef struct HashSet HashSet;

struct HashSet
{
  char      *key;
  int       key_len;
  void      *data;
  HashSet   *next;
  HashSet   *prev;
  void      (*hash_set_data_dump)(void *);
};

typedef struct HashTable
{
  unsigned int range;
  HashSet **table;
} HashTable;

HashTable *hash_tbl_create (unsigned int range);
void hash_tbl_delete (HashTable *table);
int hash_tbl_insert (HashTable *table, const char *key, int key_len, void *data,
                    void (*hash_set_data_dump)(void *));
int hash_tbl_remove (HashTable *table, const char *key, int key_len);
void hash_tbl_dump (HashTable *table);
int hash_tbl_get (HashTable* table, const char *key, void **data);
int hash_tbl_find (HashTable *table, const char *key, HashSet **set);

#endif /* __HASH_TABLE_H__ */