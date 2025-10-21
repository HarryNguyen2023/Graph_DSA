#ifndef __STACK_H__
#define __STACK_H__

typedef struct StackNode
{
  void* data;
  struct StackNode* next;
} StackNode;

typedef struct Stack
{
  int capacity;
  int depth;
  StackNode* top;
} Stack;

Stack* stack_create (int capacity);
int stack_delete (Stack* stack);
int stack_is_empty (Stack* stack);
int stack_push (Stack* stack, void* data);
void* stack_pop (Stack* stack);
void stack_dump (Stack* stack, void (*stack_dump_node)(void *));

#endif /* __STACK_H__ */