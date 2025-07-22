#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "buffer.h"

buffer * createBuffer(char * path, size_t size){
    
    //allocates
    buffer * b = (buffer*)malloc(sizeof(buffer));
    b->halo = 256;

    //sentinel clauses
    FILE * fid = fopen(path,"r");
    if(fid == NULL){
        fprintf(stderr,"unable to open file %s exiting...",path);
        free(b);
        exit(1);
    }
    if(size < b->halo){
        fprintf(stderr,"buffer can't be smaller than %zu",b->halo);
        free(b);
        exit(1);
    }

    //initialization
    b->line = 1;
    b->at = 0;
    b->size = size;
    b->done = 0;
    b->fid = fid;
    b->column = 0;
    b->prev_column = 0;
    b->chunk = (char*)malloc(sizeof(char) * size);
    return b;
}

void copyLast(buffer * b, char ** copy, size_t size){
    if(b->at < size){
        fprintf(stderr,"copy length bigger than available!");
        exit(1);
    }

    *copy = realloc(*copy,sizeof(char) * (size + 1));
    memcpy(*copy,&b->chunk[b->at - size - 1], size + 1);
    (*copy)[size] = '\0';
}

char getChar(buffer * b){
    
    //return EOF if the whole file was parsed
    if(b->done)
        return EOF;

    //need to fetch more letters?
    if(!(b->at % b->size)){

        //copy the halo for the unget
        size_t offset = b->size - b->halo;
        for(size_t i = 0; i < b->halo; i++)
            b->chunk[i] = b->chunk[i + offset]; 

        //copy the new letters
        char c = 0;
        for(size_t i = b->halo; i < b->size && c != EOF; i++){
            c = getc(b->fid);
            b->chunk[i] = c;
        }
        b->at = b->halo;
    }

    //return more letters
    char c = b->chunk[b->at];
    b->done = c == EOF;
    b->prev_column = b->column;
    if(c == '\n'){
        b->line++;
        b->column = 0;
    }
    else
        b->column++;
    b->at++;

    return c;
}

void ungetChar(buffer * b){
    //bound check
    if(!b->at)
        return;
    
    //go to the previous character
    b->at--;
    b->line -= b->chunk[b->at] == '\n';
    b->column = b->prev_column; //can get the previous column once
}

void destroyBuffer(buffer * b){
    fclose(b->fid);
    free(b->chunk);
    free(b);
}
