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
    size_t * declaration_stack;
    size_t stack_size;
    size_t top;
}intermediary;


intermediary * createIntermediary(parser * par, int quiet);
void destroyIntermediary(intermediary * inter);
void generate(intermediary * inter, node * root);
void declare(intermediary * inter);
void define(intermediary * inter);
#endif