#include "quadruple.h"
#include "dgenerate/states_dfa.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


quadruple createQuadruple(dfa_states instruction, char * destination, char * source_A, char * source_B, int quiet){
    quadruple quad = {
        .instruction = instruction,
        .destination = destination != NULL ? (char*)malloc(sizeof(char) * (strlen(destination)+1)) : NULL,
        .source_A = source_A != NULL ? (char*)malloc(sizeof(char) * (strlen(source_A)+1)) : NULL,
        .source_B = source_B != NULL ? (char*)malloc(sizeof(char) * (strlen(source_B)+1)) : NULL,
        .quiet = quiet
    };
    
    if(destination != NULL) strcpy(quad.destination,destination);
    if(source_A != NULL) strcpy(quad.source_A,source_A);
    if(source_B != NULL) strcpy(quad.source_B,source_B);
    return quad;
}

void destroyQuadruple(quadruple quad){
    if(!quad.quiet)
        printQuadruple(quad);

    free(quad.destination);
    free(quad.source_A);
    free(quad.source_B);
}

void sprintQuadruple(quadruple quad, char * string){
    size_t size = strlen(STATE_NAMES[quad.instruction - START]) +
                  strlen(quad.destination) +
                  strlen(quad.source_A) +
                  strlen(quad.source_B) + 6;
    string = realloc(string, size);
    sprintf(string,"%s(%s,%s,%s)",STATE_NAMES[quad.instruction-START], quad.destination, quad.source_A, quad.source_B);
}

void printQuadruple(quadruple quad){
    printf("%s(%s,%s,%s)\n",STATE_NAMES[quad.instruction-START], quad.destination, quad.source_A, quad.source_B);
}
