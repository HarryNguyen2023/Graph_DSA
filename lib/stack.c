#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"

Stack* stack_create (void) 
{
  Stack *stack = (Stack *)malloc(sizeof(Stack));
  if (! stack)
  {
    printf("Error: Could not allocate memory for stack\n");
    return NULL;
  }
  stack->depth = 0;
  stack->top = NULL;
  return stack;
}

int stack_delete (Stack* stack) 
{
  if (stack == NULL)
  {
    printf("[%s,%d] Error: Stack is NULL\n", __func__, __LINE__);
    return -1;
  }

  StackNode* current = stack->top;
  StackNode* next = NULL;
  while (current != NULL)
  {
    next = current->next;
    free(current);
    current = next;
  }
  free(stack);
  stack = NULL;
  return 0;
}

int stack_is_empty (Stack* stack) 
{
  if (stack == NULL)
  {
    printf("[%s,%d] Error: Stack is NULL\n", __func__, __LINE__);
    return -1;
  }
  return stack->depth == 0;
}

int stack_push (Stack* stack, void *data) 
{
  if (stack == NULL)
  {
    printf("[%s,%d] Error: Stack is NULL\n", __func__, __LINE__);
    return -1;
  }

  StackNode* newNode = (StackNode*)malloc(sizeof(StackNode));
  if (! newNode)
  {
    printf("Error: Could not allocate memory for stack node\n");
    return -1;
  }
  newNode->data = data;
  newNode->next = stack->top;
  stack->top = newNode;
  stack->depth++;
  return 0;
}

void* stack_pop (Stack* stack) 
{
  void* data = NULL;

  if (stack == NULL)
  {
    printf("[%s,%d] Error: Stack is NULL\n", __func__, __LINE__);
    return NULL;
  }

  if (stack_is_empty(stack))
  {
    printf("Error: Stack is empty\n");
    return NULL;
  }

  StackNode* temp = stack->top;
  data = temp->data;
  stack->top = stack->top->next;
  free(temp);
  stack->depth--;
  return data;
}