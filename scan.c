#include "dgenerate/states_dfa.h"
#include "dgenerate/dfa.h"
#include <stdio.h>
#include <stdlib.h>

void scan(unsigned char c){
    static dfa_states state = START;
    state = DFA[state][c];
    printf("%d\n",state);
    printf("%d\n",FAILURE);
}