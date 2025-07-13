#ifndef DICT_H
#define DICT_H

#include <stddef.h>
#include <string.h>
#include "stack.h"

typedef struct{
    entry_t entry;
    uint64_t transitions[2];
    size_t decedents[128];
    char filled;
}dict_t;

typedef struct dictionary{
    dict_t * arena;
    size_t height;
    size_t capacity;
    stack leafs;
}dictionary;

dictionary createDict();
void destroyDict(dictionary * d);
entry_t queryDict(dictionary * d, char * name, int * found, size_t root, size_t * leaf);
int insertDict(dictionary * d, char * name, entry_t e, size_t root, size_t * leaf);
void traverseDict(dictionary * d, size_t root);
entry_t nextDict(dictionary * d, int * bottom);
void resetDict(dictionary * d);
#endif