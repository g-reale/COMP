#include "stack.h"
#include <stdlib.h>
#include <stdio.h>

stack createStack(){
    stack s = {
        .stack = (entry_t*) malloc(sizeof(entry_t) * INITIAL_CAPACITY),
        .top = (entry_t){0},
        .capacity = INITIAL_CAPACITY,
        .height = 0,
        .at = 0,
    };

    return s;
}

void destroyStack(stack * s){
    free(s->stack);
    s->stack = NULL;
}

void push(stack * s, entry_t e){
    
    s->height++;

    //reallocate memory if needed
    if(s->height == s->capacity){
        s->capacity *= 2;
        s->stack = realloc(s->stack, sizeof(entry_t) * s->capacity);
    }
    s->stack[s->height] = e;
    s->top = e;
}

entry_t pop(stack * s, int * bottom){
    
    if(bottom)
        *bottom = 0;
    
    if(s->height == 0){
        if(bottom) *bottom = 1;
        s->top = s->stack[0];
        return s->stack[0];
    }
    
    entry_t e = s->stack[s->height];
    s->height--;
    s->top = s->stack[s->height];
    return e;
}

entry_t next(stack * s, int * bottom){

    if(1 == s->at){
        if(bottom) *bottom = 1;
        return s->stack[1];
    }

    if(bottom) *bottom = 0;
    entry_t e = s->stack[s->at];
    s->at--;
    return e;
}

void reset(stack * s){
    s->at = s->height;
}