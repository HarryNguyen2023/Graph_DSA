#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash_table.h"

HashTable *hash_tbl_create (unsigned int range)
{
  HashTable *table = NULL;
  int i;

  if (! range)
    return NULL;

  table = (HashTable *)malloc(sizeof (HashTable));
  if (! table)
  {
    printf ("[%s,%d] Fail to allocate memory for hash table\n", __func__, __LINE__);
    return NULL;
  }

  table->range = range;
  table->table = (HashSet **)malloc(range * sizeof (HashSet *));
  if (! table->table)
  {
    printf ("[%s,%d] Fail to allocate memory for hash table\n", __func__, __LINE__);
    goto ERR_EXIT1;
  }

  for (i = 0; i < range; ++i)
    table->table[i] = NULL;

  return table;

ERR_EXIT1:
  if (table)  free(table);
  table = NULL;
  return NULL;
}

void hash_set_delete (HashSet *set)
{
  if (! set)
    return;

  if (set->key)
    free (set->key);
  set->key = NULL;
  free (set);
  set = NULL;
}

void hash_tbl_delete (HashTable *table)
{
  int i;
  HashSet *temp = NULL;

  if (! table)
    return;

  if (table->table)
  {
    for (i = 0; i < table->range; ++i)
    {
      if (table->table[i])
      {
        temp = table->table[i];
        while (temp->next != NULL)
          temp = temp->next;

        if (temp->prev)
        {
          do
          {
            temp = temp->prev;
            hash_set_delete (temp->next);
            temp->next = NULL;
          } while (temp->prev);
        }
        hash_set_delete (temp);
        temp = NULL;
        table->table[i] = NULL;
      }
    }
    free (table->table);
  }

  table->table = NULL;
  table->range = 0;
  free (table);
  table = NULL;
  return;
}

unsigned int djb2_hash(const char *key)
{
  unsigned int hash = 5381;
  int c;
  while ((c = *key))
  {
    hash = ((hash << 5) + hash) + c;
    key++;
  }
  return hash;
}

unsigned int hash_tbl_hash_func (HashTable *table, const char *key)
{
  if (! table || !key)
    return 0;

  return (djb2_hash(key) % table->range);
}

HashSet *hash_set_create (const char *key, int key_len, void *data, void (*hash_set_data_dump)(void *))
{
  HashSet *set = NULL;

  if (! key || ! key_len)
    return NULL;
  
  set = (HashSet *)malloc(sizeof (HashSet));
  if (! set)
  {
    printf ("[%s,%d] Fail to allocate memory for hash set\n", __func__, __LINE__);
    return NULL;
  }

  set->key = (char *)malloc((key_len + 1) * sizeof(char));
  if (! set->key)
  {
    printf ("[%s,%d] Fail to allocate memory for hash set string key\n", __func__, __LINE__);
    goto ERR_EXIT;
  }
  memset (set->key, 0, (key_len + 1) * sizeof(char));
  strncpy (set->key, key, key_len);
  set->key_len = key_len;
  set->data = data;
  set->next = NULL;
  set->prev = NULL;
  set->hash_set_data_dump = hash_set_data_dump;

  return set;

ERR_EXIT:
  if (set) free(set);
  set = NULL;
  return NULL;
}

int hash_tbl_insert (HashTable *table, const char *key, int key_len, void *data,
                    void (*hash_set_data_dump)(void *))
{
  HashSet *set = NULL, *temp = NULL;
  unsigned int hash_index = 0;

  if (! table || ! key)
    return -1;

  hash_index = hash_tbl_hash_func (table, key);
  set = hash_set_create (key, key_len, data, hash_set_data_dump);
  if (! set)
  {
    printf ("[%s,%d] Fail to create new hash set\n", __func__, __LINE__);
    return -1;
  }

  if (table->table[hash_index] == NULL)
  {
    table->table[hash_index] = set;
  }
  else
  {
    temp = table->table[hash_index];
    while (temp->next != NULL)
    {
      temp = temp->next;
    }

    temp->next  = set;
    set->prev   = temp;
  }

  return 0;
}

int hash_tbl_find (HashTable *table, const char *key, HashSet **set)
{
  HashSet *temp = NULL;
  unsigned int hash_index = 0;

  if (! table  || ! table->table|| ! key)
    return -1;
  
  hash_index = hash_tbl_hash_func (table, key);
  if (! table->table[hash_index])
  {
    printf ("[%s,%d] Key %s not exist in hash table\n", __func__, __LINE__, key);
    return -1;
  }

  temp = table->table[hash_index];
  while (temp && strncmp (temp->key, key, temp->key_len))
    temp = temp->next;

  if (! temp)
  {
    printf ("[%s,%d] Key %s not exist in hash table\n", __func__, __LINE__, key);
    return -1;
  }

  *set = temp;
  return 0;
}

int hash_tbl_remove (HashTable *table, const char *key, int key_len)
{
  HashSet *temp = NULL;
  int rv = 0;
  unsigned int hash_index = 0;

  if (! table  || ! table->table|| ! key)
    return -1;

  rv = hash_tbl_find(table, key, &temp);
  if (rv != 0 || temp == NULL)
  {
    printf ("[%s,%d] Key %s not exist in hash table\n", __func__, __LINE__, key);
    return -1;
  }

  if (temp->prev)
    temp->prev->next = temp->next;
  else
    table->table[hash_index] = temp->next;

  if (temp->next)
    temp->next->prev = temp->prev;

  hash_set_delete (temp);
  temp = NULL;

  return 0;
}

void hash_tbl_dump (HashTable *table)
{
  int i;
  HashSet *temp = NULL;

  if (! table || ! table->range || ! table->table)
    return;

  for (i = 0; i < table->range; ++i)
  {
    if (table->table[i])
    {
      temp = table->table[i];
      while (temp)
      {
        printf ("Key: %s - Data: ", temp->key);
        if (temp->data
            && temp->hash_set_data_dump)
          (*temp->hash_set_data_dump)(temp->data);

        temp = temp->next;
      }
    }
  }

  return;
}

int hash_tbl_get (HashTable* table, const char *key, void **data)
{
  HashSet *temp = NULL;
  int rv = 0;
  unsigned int hash_index = 0;

  if (! table  || ! table->table|| ! key)
    return -1;

  rv = hash_tbl_find(table, key, &temp);
  if (rv != 0 || temp == NULL)
  {
    printf ("[%s,%d] Key %s not exist in hash table\n", __func__, __LINE__, key);
    return -1;
  }

  *data = temp->data;
  return 0;
}