#include "semantis.h"

semantis * createSemantis(size_t context_size, int quiet){
    semantis * s = (semantis*)malloc(sizeof(semantis));
    s->context_stack = (context**)malloc(sizeof(context*));
    s->context_size = context_size;
    s->context_stack[0] = createContext(context_size,"root",0,quiet);
    s->top = 0;

    size_t index = hash("input",s->context_size);
    variable ** list = &s->context_stack[0]->variables[index];
    size_t * size = &s->context_stack[0]->var_count[index];
    insert(list,size,INT,"input",OPEN_ROUND,0);

    index = hash("output",s->context_size);
    list = &s->context_stack[0]->variables[index];
    size = &s->context_stack[0]->var_count[index];
    insert(list,size,VOID,"output",OPEN_ROUND,0);
    
    s->type = FAILURE;
    s->lexeme = (char*)malloc(0);
    s->category = FAILURE;
    s->arg_count = 0;

    s->state = BEGIN;
    s->main_declared = 0;
    s->success = 1;
    s->quiet = quiet;
    return s;
}

void destroySemantis(semantis * s){
    if(!s->main_declared)
        fprintf(stderr,"ERRO SEMANTICO: main não foi declarada\n");
    for(size_t i = 0; i <= s->top; i++)
        destroyContext(s->context_stack[i]);
    free(s->context_stack);
    free(s->lexeme);
    free(s);
}

void stackContext(semantis * s, int explicit, int anonymous){
    s->context_stack = (context**)realloc(s->context_stack,sizeof(context*) * (s->top + 2));
    
    if(!anonymous){
        s->context_stack[s->top + 1] = createContext(s->context_size,s->lexeme,explicit,s->quiet);
    }
    else{
        s->lexeme = (char*)realloc(s->lexeme,sizeof(char) * (strlen(s->context_stack[s->top]->name) + 2));
        strcpy(s->lexeme,s->context_stack[s->top]->name);
        strcat(s->lexeme,"_");
        s->context_stack[s->top + 1] = createContext(s->context_size,s->lexeme,explicit,s->quiet);
    }
    s->top++;
}

void popContext(semantis * s, size_t line){
    if(!s->top)
        return;
    
    int returned = s->context_stack[s->top]->returned;
    if(!returned && s->context_stack[s->top]->explicit){
        fprintf(stderr,"ERRO SEMANTICO: função inteira \"%s\" encerra sem retorno LINHA: %lld\n",
        s->context_stack[s->top]->name,
        line
    );
        s->success = 0;
    }

    destroyContext(s->context_stack[s->top]);
    s->context_stack = (context**)realloc(s->context_stack,sizeof(context*) * s->top);
    s->top--;
    s->context_stack[s->top]->returned = returned;
}

variable * declareVar(semantis * s, size_t line){
    static const size_t main_hash = 603;    
    
    size_t index = hash(s->lexeme,s->context_size);
    variable ** list = &s->context_stack[s->top]->variables[index];
    size_t * size = &s->context_stack[s->top]->var_count[index];
    variable * var;

    if(isUnique(*list,*size,s->lexeme) != NULL){
        fprintf(stderr,"ERRO SEMANTICO: simbolo \"%s\" já foi declarado nesse contexto LINHA: %lld\n",s->lexeme,line);
        s->success = 0;
        return NULL;
    }
    else
        var = insert(list,size,s->type,s->lexeme,s->category,line);
    
    //stack a new context with the name of the function
    if(s->category == OPEN_ROUND){

        if(s->main_declared){
            fprintf(stderr,"ERRO SEMANTICO: funcão \"%s\" declarada após a main LINHA: %lld\n",s->lexeme,line);
            s->success = 0;
        }

        if(index == main_hash && strcmp(s->lexeme,"main") == 0){
            if(s->type != VOID){
                fprintf(stderr,"ERRO SEMANTICO: função main deve retornar void LINHA: %lld\n",line);
                s->success = 0;
            }
            s->main_declared = 1;
        }
        stackContext(s,s->type == INT,0);
    }

    return var;
}

variable * searchVar(semantis * s, size_t line){
    size_t index = hash(s->lexeme, s->context_size);
    variable * var = NULL;
    size_t i = s->top;

    //search on contexts
    while(var == NULL){
        variable * list = s->context_stack[i]->variables[index];
        size_t size = s->context_stack[i]->var_count[index];
        var = isUnique(list,size,s->lexeme);
        if(i == 0) break;
        i--;
    }

    if(var == NULL){
        fprintf(stderr,"ERRO SEMANTICO: simbolo \"%s\" utilizado mas nunca declarado LINHA: %lld\n",s->lexeme,line);
        s->success = 0;
        return NULL;
    }

    useVar(var,line);
    return var;
}

//use a DFA to choose the semantis's actions
void analise(semantis * s, dfa_states terminal, char * lexeme, size_t line){
    
    // static const char * state_names[] = {"BEGIN","DECL_1","DECL_2","USE_1","USE_2"};
    // printf("called analise %s %s\n",state_names[s->state],STATE_NAMES[terminal-START]);

    switch(s->state){

        case BEGIN:
            switch (terminal){
                
                case RETURN:
                    s->context_stack[s->top]->returned = 1;
                    s->state = BEGIN;
                    break;

                case OPEN_CURLY:
                    stackContext(s,0,1);
                    s->state = BEGIN;
                    break;
                
                case CLOSE_CURLY:
                    popContext(s,line);
                    s->state = BEGIN;
                    break;

                case IDENTIFIER:
                    s->lexeme = (char*)realloc(s->lexeme,sizeof(char) * (strlen(lexeme) + 1));
                    strcpy(s->lexeme,lexeme);
                    s->state = USE_1;
                    break;

                case INT:
                case VOID:
                    s->type = terminal;
                    s->state = DECL_1;
                    break;
                
                default:
                    s->state = BEGIN;
            }
            break;

        case USE_1:
            switch (terminal){
                
                case OPEN_ROUND:
                case OPEN_SQUARE:
                    s->category = terminal;
                    searchVar(s,line);
                    s->state = BEGIN;
                    break;

                default:
                    s->category = SEMI;
                    searchVar(s,line);
                    s->state = BEGIN;
                    break;
            }
        break;

        case DECL_1:
            switch (terminal){

                case IDENTIFIER:
                    s->lexeme = (char*)realloc(s->lexeme,sizeof(char) * (strlen(lexeme) + 1));
                    strcpy(s->lexeme,lexeme);
                    s->state = DECL_2;
                    break;
                
                default:
                    s->state = BEGIN;
                    break;
            }
        break;

        case DECL_2:
            switch (terminal){

                case OPEN_ROUND:
                case OPEN_SQUARE:
                    s->category = terminal;
                    declareVar(s,line);
                    s->state = BEGIN;
                    break;
                
                case SEMI:
                case CLOSE_ROUND:
                case COMMA:
                    s->category = SEMI;
                    declareVar(s,line);
                    s->state = BEGIN;
                    s->state = BEGIN;
                    break;
                
                default:
                    s->state = BEGIN;
            }
        break;
    }
}