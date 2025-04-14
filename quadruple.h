#ifndef QUADRUPLE_H
#define QUADRUPLE_H

#include "states_dfa.h"

// typedef enum{
//     VOID_FUNCTION,
//     INT_FUNCTION,
//     SCALAR_ARGUMENT,
//     VECTOR_ARGUMENT,
// }pseudo_instucion;

typedef struct quadruple{
    dfa_states instruction;
    char * destination;
    char * source_A;
    char * source_B;
    int quiet;
}quadruple;

quadruple createQuadruple(dfa_states instruction, char * destination, char * source_A, char * source_B, int quiet);
void destroyQuadruple(quadruple quad);
void sprintQuadruple(quadruple quad, char * string);
void printQuadruple(quadruple quad);
#endif