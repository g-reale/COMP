#ifndef STACK_H
#define STACK_H

#include <stddef.h>
#include <stdint.h>

#define INITIAL_CAPACITY 40

typedef enum {
    VARIABLE,
    FUNCTION,
    PPOINTER,
    CONSTANT,
    CONTEXT,
    VECTOR
} type_t;

typedef struct {
    size_t address;
    size_t size;
    size_t value;
} data_t;

typedef struct {
    type_t type;
    data_t data;
} entry_t;

typedef struct stack {
    entry_t * stack;
    entry_t top;
    size_t capacity;
    size_t height;
    size_t at;
} stack;

stack createStack();
void destroyStack(stack * s);
void push(stack * s, entry_t e);
entry_t pop(stack * s, int * bottom);
entry_t next(stack * s, int * bottom);
void reset(stack * s);
#endif