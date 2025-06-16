#ifndef DICT_H
#define DICT_H

#include <stddef.h>
#include <string.h>

typedef struct entry {
    char * name;
    void * value;
    void (*destructor)(void *);
    struct entry * next;
} entry;

typedef struct dictionary {
    entry **page;
    size_t size;
    size_t (*mapping)(char *, size_t);
} dictionary;

dictionary createDict(size_t size, size_t (*mapping)(char * string, size_t));
void deleteDict(dictionary dict);
entry ** inDict(dictionary dic, char * name, int * found);
void * getDict(dictionary dict, char * name, int * found);
int insertDict(dictionary dict, char * name, void * value, void (*destructor)(void *));
#endif