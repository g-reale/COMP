#ifndef INTERMEDIARY_H
#define INTERMEDIARY_H

#include "quadruple.h"
#include "parser.h"


typedef struct intermediary{
    quadruple * pseudo_asm;
    size_t code_size;
    const node * root;
    size_t anonymous_index;
    char * anonymous;
    int quiet;
}intermediary;

intermediary * createIntermediary(parser * par, int quiet);
void destroyIntermediary(intermediary * inter);
void generate(intermediary * inter, node * root);
#endif