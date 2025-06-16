#include "stack.h"

stack createStack(void (*destructor)(void *)) {
    stack s = {
        .stack = (void **)malloc(sizeof(void *) * INITIAL_CAPACITY),
        .destructor = destructor,
        .height = 0,
        .at = 0,
        .capacity = INITIAL_CAPACITY
    };
    return s;
}

void destroyStack(stack * s) {
    for (size_t i = 0; i < s->height; i++)
        s->destructor(s->stack[i]);

    free(s->stack);
    s->stack = NULL;
    s->height = 0;
    s->capacity = 0;
    s->at = 0;
}

void push(stack * s, void * value) {
    if (s->height == s->capacity) {
        s->capacity *= 2;
        s->stack = realloc(s->stack, sizeof(void *) * s->capacity);
    }
    s->stack[s->height++] = value;
}

void * pop(stack * s) {
    if (s->height == 0)
        return NULL;

    void *data = s->stack[--s->height];
    return data;
}

void reset(stack * s) {
    s->at = s->height;
}

void * next(stack * s) {
    if (s->at == 0)
        return NULL;

    return s->stack[--s->at];
}

void * top(stack * s){
    return s->stack[s->height-1];
}