#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "circular_buff.h"

CirBuffer *cirbuff_create (int capacity, int element_size)
{
  CirBuffer *new_buf;

  if (!capacity || !element_size)
    return NULL;

  new_buf = (CirBuffer *)malloc(sizeof (CirBuffer) + capacity);
  if (! new_buf)
  {
    printf ("[%s,%d] Fail to allocate memory for new circular buffer\n",
            __func__, __LINE__);
    return NULL;
  }

  memset (new_buf->buff, 0, capacity);
  new_buf->capacity     = capacity;
  new_buf->element_size = element_size;
  new_buf->head         = -1;
  new_buf->rear         = -1;
  return new_buf;
}

void cirbuff_delete (CirBuffer* buff)
{
  if (! buff)
    return;

  printf ("[%s,%d] Deleting circular buffer with capacity %d, element size %d\n",
          __func__, __LINE__, buff->capacity, buff->element_size);
  
  free (buff);
  return;
}

int cirbuff_is_empty (CirBuffer* buff)
{
  if (! buff)
    return 0;

  return (buff->head == -1);
}

int cirbuff_is_full (CirBuffer* buff)
{
  if (! buff)
    return 0;

  return (buff->head == buff->rear
          && buff->head != -1);
}

int cirbuff_avail_size (CirBuffer* buff)
{
  if (!buff)
    return 0;

  if (buff->head > buff->rear)
    return (buff->capacity - (buff->head - buff->rear));
  else if (buff->head < buff->rear)
    return (buff->rear - buff->head);
  else if (cirbuff_is_empty (buff))
    return buff->capacity;

  return 0;
}

int cirbuff_write (CirBuffer* buff, unsigned char* data, int element_num)
{
  int avail_size = 0, write_size = 0, tail_size = 0;

  if (!buff || !data || !element_num)
    return 0;

  if (cirbuff_is_full (buff))
    return 0;

  avail_size = cirbuff_avail_size (buff);
  if (avail_size < buff->element_size)
    return 0;

  if (cirbuff_is_empty (buff))
  {
    buff->head = 0;
    buff->rear = 0;
  }
  write_size = element_num * buff->element_size;
  write_size = (write_size > avail_size) ? \
                  avail_size - (avail_size % buff->element_size) : write_size;
  tail_size  = buff->capacity - buff->head;
  if (tail_size > write_size)
    memcpy ((void *)(buff->buff + buff->head), (void *)data, write_size);
  else
  {
    memcpy ((void *)(buff->buff + buff->head), (void *)data, tail_size);
    memcpy ((void *)(buff->buff), (void *)(data + tail_size), write_size - tail_size);
  }

  buff->head = (buff->head + write_size) % buff->capacity;

  return (write_size / buff->element_size);
}

int cirbuff_read (CirBuffer* buff, unsigned char* data, int element_num)
{
  int avail_size = 0, read_size = 0, tail_size = 0;

  if (!buff || !data || !element_num)
    return 0;

  if (cirbuff_is_empty (buff))
    return 0;

  avail_size = buff->capacity - cirbuff_avail_size (buff);
  if (avail_size < buff->element_size)
    return 0;

  read_size = element_num * buff->element_size;
  read_size = (read_size > avail_size) ? \
                (avail_size - (avail_size % buff->element_size)) : read_size;
  tail_size  = buff->capacity - buff->rear;
  if (tail_size > read_size)
  {
    memcpy ((void *)data, (void *)(buff->buff + buff->rear), read_size);
  }
  else
  {
    memcpy ((void *)data, (void *)(buff->buff + buff->rear), tail_size);
    memcpy ((void *)(data + tail_size), (void *)(buff->buff), read_size - tail_size);
  }

  buff->rear = (buff->rear + read_size) % buff->capacity;
  if (buff->rear == buff->head)
  {
    buff->rear = -1;
    buff->head = -1;
  }

  return (read_size / buff->element_size);
}

void cirbuff_dump (CirBuffer *buff, void (*cirbuff_element_print)(void *))
{
  int i, count = 0;

  if (!buff)
    return;

  if (cirbuff_is_empty (buff))
  {
    printf ("[%s,%d] Circular buffer is empty\n", __func__, __LINE__);
    return;
  }
  printf ("Circular buffer: ");

  i = buff->rear;
  do {
    if ((cirbuff_element_print))
      (*cirbuff_element_print)(buff->buff + i);
    i = (i + buff->element_size) % buff->capacity;
    count += buff->element_size;
  } while (i != buff->head && count < buff->capacity);

  printf ("\n");
}