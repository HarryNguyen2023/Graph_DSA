#ifndef __THREAD_H__
#define __THREAD_H__

#define HAVE_THREAD_SELECT

typedef enum EventType
{
  EVENT_EVENT,
  EVENT_TIMER,
  EVENT_READ,
  EVENT_WRITE
} EventType;

typedef struct Event
{
  EventType type;
  void      (*cb_func) (void *);
  void      *input;
  union
  {
    int           fd;
    struct
    {
      struct timeval time_now;
      struct timeval time_val;
    }time;
    unsigned int  event;
  } data;
} Event;

typedef struct EventLoop
{
  Queue*      high_prio_queue;
  Queue*      mid_prio_queue;
  Queue*      low_prio_queue;
  Queue*      timer_queue;
  HashTable*  read_event;
  HashTable*  write_event;
#ifdef HAVE_THREAD_SELECT
  fd_set      readfds;
  fd_set      writefds;
#endif /* HAVE_THREAD_SELECT */
} EventLoop;

#define EVENT_QUEUE_TRAVERSE(Q,N,E)               \
  for (N = (Q)->front; N != NULL; N = (N)->next)  \
    if (E = (Event *)N)

#endif /* __THREAD_H__*/