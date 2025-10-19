#ifndef __CIRCULAR_BUFFER_H__
#define __CIRCULAR_BUFFER_H__

typedef struct CirBuffer
{
  int capacity;
  int head;
  int rear;
  int element_size;
  char buff[];
} CirBuffer;

CirBuffer *cirbuff_create (int capacity, int element_size);
void cirbuff_delete (CirBuffer* buff);
int cirbuff_is_empty (CirBuffer* buff);
int cirbuff_is_full (CirBuffer* buff);
int cirbuff_write (CirBuffer* buff, unsigned char* data, int element_num);
int cirbuff_read (CirBuffer* buff, unsigned char* data, int element_num);
void cirbuff_dump (CirBuffer *buff, void (*cirbuff_element_print)(void *));

#endif /* __CIRCULAR_BUFFER_H__ */