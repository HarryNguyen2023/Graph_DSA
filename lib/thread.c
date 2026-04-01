#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#else /* LINUX */
  #include <sys/select.h>
  #include <sys/socket.h>
  #include <sys/time.h>
  #include <netinet/in.h>
#endif
#include "queue.h"
#include "hash_table.h"
#include "thread.h"

#define THREAD_MAX_SIZE     (20)

struct timeval thread_queue_overtime = {
  .tv_sec  = 0,
  .tv_usec = 500000
};

int gettimeofday_windows(struct timeval *tv, void *tz) {
  // According to Microsoft documentation, GetSystemTimeAsFileTime is the way to go.
  // The FILETIME structure is a 64-bit value representing the number of
  // 100-nanosecond intervals since Jan 1, 1601 UTC.
  FILETIME ft;
  GetSystemTimeAsFileTime(&ft);

  // Convert FILETIME to 64-bit integer
  // The cast to int64_t handles potential type issues
  int64_t micro_seconds = (int64_t)ft.dwHighDateTime << 32 | ft.dwLowDateTime;

  // Convert from 100-nanosecond intervals to microseconds
  micro_seconds /= 10;

  // The Unix epoch (Jan 1, 1970) is 11644473600 seconds after the Windows epoch (Jan 1, 1601)
  // 11644473600 seconds * 1,000,000 microseconds/second = 11,644,473,600,000,000 microseconds
  const int64_t EPOCH_DIFF = 11644473600000000LL;

  // Subtract the epoch difference
  micro_seconds -= EPOCH_DIFF;

  // Fill the timeval struct
  tv->tv_sec = (long)(micro_seconds / 1000000LL);
  tv->tv_usec = (long)(micro_seconds % 1000000LL);

  // The tzp argument (time zone information) is ignored for simplicity
  return 0; // 0 on success
}

int timeval_sub(const struct timeval *tv_last, const struct timeval *tv_now, struct timeval *res)
{
  long int total_us_tv_now, total_us_tv_last, total_us_tv_res;

  if (! tv_last || ! tv_now || ! res)
    return -1;

  total_us_tv_now  = (tv_now->tv_sec * 1000000LL) + tv_now->tv_usec;
  total_us_tv_last = (tv_last->tv_sec * 1000000LL) + tv_last->tv_usec;
  total_us_tv_res  = total_us_tv_now - total_us_tv_last;
  if (total_us_tv_res < 0)
    return -1;

  res->tv_sec  = (total_us_tv_res / 1000000LL);
  res->tv_usec = (total_us_tv_res % 1000000LL);
  return 0;
}

EventLoop* thread_init (void)
{
  EventLoop *event_loop = NULL;
  event_loop = (EventLoop *)malloc(sizeof(EventLoop));
  if (event_loop == NULL)
  {
    printf ("[%s,%d]Error: Fail to allocate memory for event loop!", __func__, __LINE__);
    return NULL;
  }

  memset(event_loop, 0, sizeof(EventLoop));
  event_loop->high_prio_queue = queue_create(THREAD_MAX_SIZE);
  if (! event_loop->high_prio_queue)
  {
    printf ("[%s,%d]Error: Fail to allocate memory for high prio event loop!", __func__, __LINE__);
    goto EXIT;
  }

  event_loop->mid_prio_queue = queue_create(THREAD_MAX_SIZE);
  if (! event_loop->mid_prio_queue)
  {
    printf ("[%s,%d]Error: Fail to allocate memory for mid prio event loop!", __func__, __LINE__);
    goto EXIT;
  }

  event_loop->low_prio_queue = queue_create(THREAD_MAX_SIZE);
  if (! event_loop->low_prio_queue)
  {
    printf ("[%s,%d]Error: Fail to allocate memory for low prio event loop!", __func__, __LINE__);
    goto EXIT;
  }

  event_loop->timer_queue = queue_create(THREAD_MAX_SIZE);
  if (! event_loop->timer_queue)
  {
    printf ("[%s,%d]Error: Fail to allocate memory for timer event loop!", __func__, __LINE__);
    goto EXIT;
  }

  event_loop->read_event = hash_tbl_create (THREAD_MAX_SIZE);
  if (! event_loop->read_event)
  {
    printf ("[%s,%d]Error: Fail to allocate memory for read event hash table!", __func__, __LINE__);
    goto EXIT;
  }

  event_loop->write_event = hash_tbl_create (THREAD_MAX_SIZE);
  if (! event_loop->write_event)
  {
    printf ("[%s,%d]Error: Fail to allocate memory for write event hash table!", __func__, __LINE__);
    goto EXIT;
  }

  return event_loop;

EXIT:
  if (event_loop->read_event)  hash_tbl_delete (event_loop->read_event);
  event_loop->read_event = NULL;
  if (event_loop->timer_queue)  queue_delete(event_loop->timer_queue);
  event_loop->timer_queue = NULL;
  if (event_loop->low_prio_queue)  queue_delete(event_loop->low_prio_queue);
  event_loop->low_prio_queue = NULL;
  if (event_loop->mid_prio_queue)  queue_delete(event_loop->mid_prio_queue);
  event_loop->mid_prio_queue = NULL;
  if (event_loop->high_prio_queue)  queue_delete(event_loop->high_prio_queue);
  event_loop->high_prio_queue = NULL;
  if (event_loop)   free (event_loop);
  event_loop = NULL;
}

Event* thread_event_create (EventType type)
{
  Event* event = NULL;

  event = (Event *)malloc(sizeof(Event));
  if (! event)
  {
    printf ("[%s,%d]Error: Fail to allocate memory for event", __func__, __LINE__);
    return NULL;
  }

  memset(event, 0, sizeof(Event));
  event->type = type;
  return event;
}

int thread_add_event (EventLoop *event_loop, const unsigned int event, void (*cb_func)(void *), void *input)
{
  Event* event_event = NULL;

  if (! event_loop || ! cb_func)
    return -1;

  event_event = thread_event_create (EVENT_EVENT);
  if (! event_event)
  {
    printf ("[%s,%d]Error: Fail to create event type event", __func__, __LINE__);
    return -1;
  }
  event_event->cb_func      = cb_func;
  event_event->input        = input;
  event_event->data.event   = event;

  return queue_enqueue (event_loop->high_prio_queue, (void *)event_event);
}

int thread_add_read (EventLoop *event_loop, const int fd, void (*cb_func)(void *), void *input)
{
  Event* event_read = NULL;
  int ret;
  char fd_key[5] = {'\0'};

  if (! event_loop || ! cb_func)
    return -1;

  snprintf(fd_key, sizeof(fd_key), "%d", fd);

  ret = hash_tbl_get (event_loop->read_event, fd_key, (void **)&event_read);
  if (ret == 0 && event_read)
  {
    event_read->cb_func  = cb_func;
    event_read->input    = input;
    event_read->data.fd  = fd;

    return 0;
  }

  event_read = thread_event_create (EVENT_READ);
  if (! event_read)
  {
    printf ("[%s,%d]Error: Fail to create event type read", __func__, __LINE__);
    return -1;
  }
  event_read->cb_func  = cb_func;
  event_read->input    = input;
  event_read->data.fd  = fd;
  
  return hash_tbl_insert(event_loop->read_event, fd_key, strlen(fd_key), event_read, NULL);
}

int thread_add_write (EventLoop *event_loop, const int fd, void (*cb_func)(void *), void *input)
{
  Event* event_write = NULL;
  int ret;
  char fd_key[5] = {'\0'};

  if (! event_loop || ! cb_func)
    return -1;

  snprintf(fd_key, sizeof(fd_key), "%d", fd);
  ret = hash_tbl_get (event_loop->write_event, fd_key, (void **)&event_write);
  if (ret == 0 && event_write)
  {
    event_write->cb_func  = cb_func;
    event_write->input    = input;
    event_write->data.fd  = fd;

    return 0;
  }

  event_write = thread_event_create (EVENT_WRITE);
  if (! event_write)
  {
    printf ("[%s,%d]Error: Fail to create event type write", __func__, __LINE__);
    return -1;
  }
  event_write->cb_func  = cb_func;
  event_write->input    = input;
  event_write->data.fd  = fd;
  
  return hash_tbl_insert(event_loop->write_event, fd_key, strlen(fd_key), event_write, NULL);
}

int thread_add_timer (EventLoop *event_loop, const struct timeval time_val, void (*cb_func)(void *), void *input)
{
  Event* event_timer = NULL;
  int ret;

  if (! event_loop || ! cb_func)
    return -1;

  event_timer = thread_event_create (EVENT_TIMER);
  if (! event_timer)
  {
    printf ("[%s,%d]Error: Fail to create event type timer", __func__, __LINE__);
    return -1;
  }
  event_timer->cb_func  = cb_func;
  event_timer->input    = input;
  
  memcpy(&event_timer->data.time.time_val, &time_val, sizeof(struct timeval));
  ret = gettimeofday_windows(&(event_timer->data.time.time_now), NULL);
  if (ret < 0)
  {
    printf ("[%s,%d]Error: Fail to get current time for event type timer!", __func__, __LINE__);
    return -1;
  }
  
  return queue_enqueue (event_loop->timer_queue, (void *)event_timer);
}

void thread_run (Event *event)
{
  if (event)
    (*event->cb_func)(event->input);
}

int thread_get (Queue* queue, Event** event)
{
  if(queue_dequeue (queue, (void **)event) == 0)
    return 1;

  return 0;
}

int thread_fetch (EventLoop *event_loop, Event **event)
{
  struct timeval tv, sub_tv;
  QueueNode *q_node;
  Event *time_event, *read_event, *write_event;
  int ret = 0, i;
  char *key, fd_key[5] = {'\0'};;
  fd_set exceptfds;
#ifdef HAVE_THREAD_SELECT
  int nfds;
#endif /* HAVE_THREAD_SELECT */

  if (! event_loop || ! event)
    return 0;

  /* Run the event with highest priority first if exist */
  if (thread_get(event_loop->high_prio_queue, event))
    return 1;

  /* If there is no high priority event, continue to check the mid priority */
  if (thread_get(event_loop->mid_prio_queue, event))
    return 1;

  /* Finally, check the low priority queue */
  if (thread_get(event_loop->low_prio_queue, event))
    return 1;

  /* Check if there is any timer event has reached timeout to add to mid priortiy queue */
  ret = gettimeofday_windows(&tv, NULL);
  if (ret == 0)
  {
    EVENT_QUEUE_TRAVERSE(event_loop->timer_queue, q_node, time_event)
    {
      ret = timeval_sub (&tv, &(time_event->data.time.time_now), &sub_tv);
      if (ret != 0)
        continue;

      /* If timer overflow, reset the current time value and add the event into the mid prio queue */
      if ((sub_tv.tv_sec >= time_event->data.time.time_val.tv_sec)
          || (sub_tv.tv_sec == time_event->data.time.time_val.tv_sec
              && sub_tv.tv_usec >= time_event->data.time.time_val.tv_usec))
      {
        memcpy(&(time_event->data.time.time_now), &tv, sizeof(struct timeval));
        queue_enqueue (event_loop->mid_prio_queue, (void *)time_event);
      }
    }
  }

  /* Check if there is any read/write event has been ready to add to low priortiy queue */
#ifdef HAVE_THREAD_SELECT
  nfds = 0;
  FD_ZERO(&event_loop->readfds);
  FD_ZERO(&event_loop->writefds);
  FD_ZERO(&exceptfds);

  HASH_TBL_TRAVERSE(event_loop->read_event, i, key, read_event)
  {
    if (read_event->data.fd >= nfds)
      nfds = read_event->data.fd + 1;

    FD_SET(read_event->data.fd, &event_loop->readfds);
  }

  HASH_TBL_TRAVERSE(event_loop->write_event, i, key, write_event)
  {
    if (write_event->data.fd >= nfds)
      nfds = write_event->data.fd + 1;

    FD_SET(write_event->data.fd, &event_loop->writefds);
  }

  ret = select(nfds, &event_loop->readfds, &event_loop->writefds, &exceptfds, &thread_queue_overtime);
  if (ret != 0)
    return 1;

  for (i = 0; i < nfds; ++i)
  {
    if (FD_ISSET(i, &event_loop->readfds))
    {
      memset(fd_key, 0, sizeof(fd_key));
      snprintf(fd_key, sizeof(fd_key), "%d", i);
      ret = hash_tbl_get(event_loop->read_event, fd_key, (void **)&read_event);
      if (ret == 0 && read_event)
      {
        queue_enqueue(event_loop->low_prio_queue, read_event);
        hash_tbl_remove (event_loop->read_event, fd_key, strlen(fd_key));
      }
    }
    else if(FD_ISSET(i, &event_loop->writefds))
    {
      memset(fd_key, 0, sizeof(fd_key));
      snprintf(fd_key, sizeof(fd_key), "%d", i);
      ret = hash_tbl_get(event_loop->write_event, fd_key, (void **)&write_event);
      if (ret == 0 && write_event)
      {
        queue_enqueue(event_loop->low_prio_queue, write_event);
        hash_tbl_remove (event_loop->write_event, fd_key, strlen(fd_key));
      }
    }
  }
#endif /* HAVE_THREAD_SELECT */

  return 1;
}