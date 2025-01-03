#ifndef BUFFER_H
#define BUFFER_H

#include <stdio.h>
#include <stddef.h>

typedef struct{
    size_t line;
    size_t at;
    size_t size;
    size_t halo;
    int done;
    FILE * fid;
    char * chunk;
} buffer;

buffer * startBuffer(char * path,size_t size);
char getChar(buffer * b);
void copyLast(buffer * b, char ** copy, size_t size);
void ungetChar(buffer * b);
void destroyBuffer(buffer * b);
#endif