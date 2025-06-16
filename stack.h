#ifndef STACK_H
#define STACK_H

#include <stddef.h>

#define INITIAL_CAPACITY 4

typedef struct stack{
    void * * stack;
    void (*destructor)(void *);    
    size_t height;
    size_t at;
    size_t capacity;
}stack;

stack createStack(void (*destructor)(void *));
void destroyStack(stack * s);
void * next(stack * s);
void reset(stack *);
void push(stack * s, void * value);
void pop(stack * s);
void * top(stack * s);
#endif