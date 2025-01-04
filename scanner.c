#include "dgenerate/states_dfa.h"
#include "dgenerate/dfa.h"
#include "buffer.h"
#include "scanner.h"

#include <stdio.h>
#include <stdlib.h>

scanner * startScanner(char * path){
    buffer * b = startBuffer(path,256); 
    scanner * s = (scanner*)malloc(sizeof(scanner));
    s->program = b;
    s->done = 0;
    return s;
}

void destroyScanner(scanner * s){
    destroyBuffer(s->program);
    free(s);
}

dfa_states getToken(scanner * s, char ** lexeme){

    //disambiguate using the keyword hash 
    static const size_t KEYWORD_AMOUNT = 6;
    static const unsigned long HASH_MAX = 1361184172;
    static const unsigned long KEYWORD_TOKENS[] = {IF,   INT,   VOID,    ELSE,    WHILE,    RETURN};
    static const unsigned long KEYWORD_HASHES[] = {2757, 81381, 1831584, 1855825, 48126587, 1361184172};


    if(s->done)
        return FAILURE;

    unsigned long hash = 0;
    size_t digit = 1;

    char c = 0;
    size_t slice = 0;
    dfa_states state = START;
    dfa_states token = FAILURE;

    while(c != EOF && state != END && state != FAILURE){
        
        //main dfa kernel
        token = state;
        c = getChar(s->program);
        state = DFA[state][(size_t)c];
        
        //if ambiguous decide via hash
        if(state == AMBIGUOUS && hash < HASH_MAX){
            hash += digit * c;
            digit *= 26;
        }

        //save the amount of chars in the lexeme
        slice += state == AMBIGUOUS || state == IDENTIFIER || state == NUM;
    }

    if(token == AMBIGUOUS){
        token =  IDENTIFIER;
        
        //perform linear search to remove the ambiguity
        //could use bsearch but for 6 keywords it just is not worth it
        if(hash <= HASH_MAX){
        for(size_t i = 0; i < KEYWORD_AMOUNT; i++){
            if(hash == KEYWORD_HASHES[i]){
                token = KEYWORD_TOKENS[i];
                break;
            }
        }}
    }
    
    if(token == IDENTIFIER || token == NUM)
        copyLast(s->program,lexeme,slice);
    
    s->done = c == EOF;     
    ungetChar(s->program); //go back one char
    return token;
}
