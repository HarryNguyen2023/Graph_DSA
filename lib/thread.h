#ifndef __THREAD_H__
#define __THREAD_H__

#define HAVE_THREAD_POLL
// #define HAVE_THREAD_SELECT

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

enum TestEvent
{
  EVENT_ADD,
  EVENT_DELETE
};

#define EVENT_QUEUE_TRAVERSE(Q,N,T,E)       \
  for (N = (Q)->front, (N != NULL) ? ((T = (N)->next)) : (T = NULL); (N != NULL); N = T, T = (T) ? ((T)->next) : NULL)    \
    if (E = (Event *)((N)->data))

EventLoop* thread_init (void);
int thread_add_event (EventLoop *event_loop, const unsigned int event,
                      void (*cb_func)(void *), void *input);
int thread_add_read (EventLoop *event_loop, const int fd,
                     void (*cb_func)(void *), void *input);
int thread_add_write (EventLoop *event_loop, const int fd,
                      void (*cb_func)(void *), void *input);
int thread_add_timer (EventLoop *event_loop, const struct timeval time_val,
                      void (*cb_func)(void *), void *input);
int thread_fetch (EventLoop *event_loop, Event **event);
void thread_run (Event *event);

#endif /* __THREAD_H__*/