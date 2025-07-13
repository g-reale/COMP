#include "stack.h"
#include <stdlib.h>
#include <stdio.h>

stack createStack(){
    stack s = {
        .stack = (entry_t*) malloc(sizeof(entry_t) * INITIAL_CAPACITY),
        .capacity = INITIAL_CAPACITY,
        .height = 0,
        .at = 1,
    };

    return s;
}

void destroyStack(stack * s){
    free(s->stack);
    s->stack = NULL;
}

void push(stack * s, entry_t e){
    
    //reallocate memory if needed
    if(s->height == s->capacity){
        s->capacity *= 2;
        s->stack = realloc(s->stack, sizeof(entry_t) * s->capacity);
    }
    s->stack[s->height] = e;
    // printf("pushing: %ld\n",s->stack[s->height].data.address);
    s->height++;
}

entry_t pop(stack * s, int * botton){
    
    if(s->height == 0){
        *botton = 1;
        return s->stack[0];
    }

    *botton = 0;
    entry_t e = s->stack[s->height-1];
    // printf("poping: %ld\n",s->stack[s->height-1].data.address);
    s->height--;
    return e;
}

entry_t next(stack * s, int * botton){

    if(s->height < s->at){
        *botton = 1;
        return s->stack[0];
    }

    *botton = 0;
    entry_t e = s->stack[s->height - s->at];
    // printf("traversing: %ld\n",s->stack[s->height - s->at].data.address);
    s->at++;
    return e;
}

void reset(stack * s){
    s->at = 1;
}