#ifndef STACK_H
#define STACK_H

#include <stddef.h>
#include <stdint.h>

#define INITIAL_CAPACITY 40

typedef enum {
    ADDRESS,
    SUBTREE
} type_t;

typedef struct {
    size_t address;
    size_t size;
} data_t;

typedef struct {
    type_t type;
    data_t entry;
} entry_t;

typedef struct stack {
    entry_t* stack;
    size_t capacity;
    size_t height;
    size_t at;
} stack;

stack createStack();
void destroyStack(stack * s);
void push(stack * s, entry_t e);
entry_t pop(stack * s, int * botton);
entry_t next(stack * s, int * botton);
void reset(stack * s);
#endif