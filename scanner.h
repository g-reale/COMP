#ifndef SCANNER_H
#define SCANNER_H

#include "dgenerate/states_dfa.h"
#include "buffer.h"

typedef struct{
    buffer * program;
    int done;
}scanner;

typedef struct {
    dfa_states token;
    char * lexeme;
}tok;

scanner * startScanner(char * path);
void destroyScanner(scanner * s);
dfa_states getToken(scanner * s, char ** lexeme);

#endif