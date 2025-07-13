#include "dgenerate/states_dfa.h"
#include "dgenerate/dfa.h"
#include "buffer.h"
#include "scanner.h"

#include <stdio.h>
#include <stdlib.h>

scanner * createScanner(char * path, int quiet){
    scanner * s = (scanner*)malloc(sizeof(scanner));
    s->lexeme = (char*)malloc(0);
    s->program = createBuffer(path,1024); 
    s->done = 0;
    s->success = 1;
    s->quiet = quiet;
    return s;
}

void destroyScanner(scanner * s){
    destroyBuffer(s->program);
    free(s->lexeme);
    free(s);
}

dfa_states getToken(scanner * s){

    //disambiguate using the keyword variables 
    static const size_t KEYWORD_AMOUNT = 6;
    static const unsigned long variables_MAX = 1361184172;
    static const unsigned long KEYWORD_TOKENS[] = {IF,   INT,   VOID,    ELSE,    WHILE,    RETURN};
    static const unsigned long KEYWORD_variablesES[] = {2757, 81381, 1831584, 1855825, 48126587, 1361184172};

    //reset after a failure
    if(s->token == FAILURE)
        s->token = START;

    if(s->done){
        s->token = FAILURE;
        return FAILURE;
    }

    unsigned long variables = 0;
    size_t digit = 1;

    char c = 0;
    size_t slice = 0;
    dfa_states state = START;
    dfa_states token = FAILURE;

    while(c != EOF && state != END && state != FAILURE){
        
        c = getChar(s->program);

        //main dfa kernel
        token = state;
        state = DFA[state][(size_t)c];
        
        //if ambiguous decide via variables
        if(state == AMBIGUOUS && variables < variables_MAX){
            variables += digit * c;
            digit *= 26;
        }

        //save the amount of chars in the lexeme
        slice += state == AMBIGUOUS || state == IDENTIFIER || state == NUM;
    }

    if(token == AMBIGUOUS){
        token =  IDENTIFIER;
        
        //perform linear search to remove the ambiguity
        //could use bsearch but for 6 keywords it just is not worth it
        if(variables <= variables_MAX){
        for(size_t i = 0; i < KEYWORD_AMOUNT; i++){
            if(variables == KEYWORD_variablesES[i]){
                token = KEYWORD_TOKENS[i];
                break;
            }
        }}
    }

    s->done = c == EOF;     
    
    if(!s->done && state == FAILURE){
        fprintf(stderr,"ERRO LEXICO: %c(%u) INVALIDO [linha: %ld], COLUNA: %ld\n",c,c,s->program->line,s->program->column);
        s->success = 0;
        return getToken(s);
    }
    
    if(token == IDENTIFIER || token == NUM){
        copyLast(s->program,&(s->lexeme),slice);
        if(!s->quiet) printf("%s \"%s\" [linha: %ld]\n",STATE_NAMES[token-START],s->lexeme,s->program->line);
    }
    else if(!s->quiet)
        printf("%s [linha: %ld]\n",STATE_NAMES[token-START],s->program->line);

    ungetChar(s->program); //go back one char
    s->token = token;
    return token;
}
