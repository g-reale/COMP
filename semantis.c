#include "semantis.h"

semantis * startSemantis(size_t context_size, int quiet){
    semantis * s = (semantis*)malloc(sizeof(semantis));
    s->context_stack = (context**)malloc(sizeof(context*));
    s->context_size = context_size;
    s->context_stack[0] = startContext(context_size,"root",0,quiet);
    s->top = 0;
    
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
        printf("ERRO SEMANTICO: main não foi declarada\n");
    for(size_t i = 0; i <= s->top; i++)
        destroyContext(s->context_stack[i]);
    free(s->context_stack);
    free(s->lexeme);
    free(s);
}

void stackContext(semantis * s, int explicit, int anonymous){
    s->context_stack = (context**)realloc(s->context_stack,sizeof(context*) * (s->top + 2));
    
    if(!anonymous){
        s->context_stack[s->top + 1] = startContext(s->context_size,s->lexeme,explicit,s->quiet);
    }
    else{
        s->lexeme = (char*)realloc(s->lexeme,sizeof(char) * (strlen(s->context_stack[s->top]->name) + 2));
        strcpy(s->lexeme,s->context_stack[s->top]->name);
        strcat(s->lexeme,"_");
        s->context_stack[s->top + 1] = startContext(s->context_size,s->lexeme,explicit,s->quiet);
    }
    s->top++;
}

void popContext(semantis * s, size_t line){
    if(!s->top)
        return;
    
    int returned = s->context_stack[s->top]->returned;
    if(!returned && s->context_stack[s->top]->explicit){
        printf("ERRO SEMANTICO: função inteira \"%s\" encerra sem retorno LINHA: %ld\n",
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
        printf("ERRO SEMANTICO: simbolo \"%s\" já foi declarado nesse contexto LINHA: %ld\n",s->lexeme,line);
        s->success = 0;
        return NULL;
    }
    else
        var = insert(list,size,s->type,s->lexeme,s->category,line);
    
    //stack a new context with the name of the function
    if(s->category == OPEN_ROUND){

        if(s->main_declared){
            printf("ERRO SEMANTICO: funcão \"%s\" declarada após a main LINHA: %ld\n",s->lexeme,line);
            s->success = 0;
        }

        if(index == main_hash && strcmp(s->lexeme,"main") == 0){
            if(s->type != VOID){
                printf("ERRO SEMANTICO: função main deve retornar void LINHA: %ld\n",line);
                s->success = 0;
            }
            s->main_declared = 1;
        }
        stackContext(s,s->type == INT,0);
    }

    return var;
}

variable * searchVar(semantis * s, size_t line, int name_only){
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
        printf("ERRO SEMANTICO: simbolo \"%s\" utilizado mas nunca declarado LINHA: %ld\n",s->lexeme,line);
        s->success = 0;
        return NULL;
    }
    else if(!name_only && var->category != s->category){
        printf("ERRO SEMANTICO: simbolo \"%s\" utilizado como categoria diferente do definido LINHA: %ld\n",s->lexeme,line);
        s->success = 0;
        return NULL;
    }
    useVar(var,line);
    return var;
}

void matchParam(semantis * s, variable * var, int constant, size_t line){
    
    if(s->func == NULL)
        return;

    if(var == NULL && !constant)
        return;
    
    if(s->func->argument_amount <= s->arg_count){
        printf("ERRO SEMANTICO: muitos argumentos para a função: \"%s\" esperava %ld tomou %ld LINHA: %ld\n",
            s->func->lexeme,
            s->func->argument_amount,
            s->arg_count + 1, 
            line
        );
        s->success = 0;
        return;
    }

    dfa_states category = constant ? SEMI : var->category;

    if(category != s->func->argument_category[s->arg_count]){
        printf("ERRO SEMANTICO: argumento %ld de %s deve ser %s não %s LINHA: %ld\n",
            s->arg_count,
            s->func->lexeme,
            STATE_NAMES[s->func->argument_category[s->arg_count] - START],
            STATE_NAMES[category - START],
            line
        );
        s->success = 0;
    }

    s->arg_count++;
}

void callFunc(semantis * s, size_t line){

    if(s->func == NULL)
        return;

    if(s->func->argument_amount != s->arg_count)
        printf("ERRO SEMANTICO: poucos argumentos para a função \"%s\" esperava %ld tomou %ld LINHA: %ld\n",
            s->func->lexeme,
            s->func->argument_amount,
            s->arg_count,
            line
        );
    s->arg_count = 0;
}

void registerArgument(semantis *s, size_t line){
    if(s->main_declared)
        printf("ERRO SEMANTICO: inserção de argumentos após a declaração da main LINHA: %ld\n",line);
    insertArgument(s->func,s->category);
}

//use a DFA to choose the semantis's actions
void analise(semantis * s, dfa_states terminal, char * lexeme, size_t line){
    // printf("called analise %u %s\n",s->state,STATE_NAMES[terminal-START]);

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
                    s->category = terminal;
                    s->func = searchVar(s,line,1);
                    s->state = USE_2;
                    break;

                case OPEN_SQUARE:
                    s->category = terminal;
                    searchVar(s,line,0);
                    s->state = BEGIN;
                    break;
                
                default:
                    s->category = SEMI;
                    searchVar(s,line,0);
                    s->state = BEGIN;
                    break;
            }
        break;

        case USE_2:
            switch (terminal){

                case IDENTIFIER:
                    s->lexeme = (char*)realloc(s->lexeme,sizeof(char) * (strlen(lexeme) + 1));
                    strcpy(s->lexeme,lexeme);
                    matchParam(s,searchVar(s,line,1),0,line);
                    break;

                case NUM:
                    matchParam(s,NULL,1,line);
                    break;

                case CLOSE_ROUND:
                    callFunc(s,line);
                    s->state = BEGIN;
                    break;

                case COMMA:
                    s->state = USE_2;
                    break;
                
                default:
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
                    s->category = terminal;
                    s->func = declareVar(s,line);
                    s->state = ARG_1;
                    break;
                

                case SEMI:
                case OPEN_SQUARE:
                    s->category = terminal;
                    declareVar(s,line);
                    s->state = BEGIN;
                    break;
                
                default:
                    s->state = BEGIN;
                    break;
            }
        break;

        case ARG_1:
            switch (terminal){

                case VOID:
                    s->state = ARG_1;
                    break;

                case CLOSE_ROUND:
                    s->state = ARG_6;
                    break;

                case INT:
                    s->type = INT;
                    s->state = ARG_2;
                    break;
                
                default:
                    s->state = BEGIN;
                    break;
            }
        break;

        case ARG_2:
            switch (terminal){
                case IDENTIFIER:
                    s->lexeme = (char*)realloc(s->lexeme,sizeof(char) * (strlen(lexeme) + 1));
                    strcpy(s->lexeme,lexeme);
                    s->state = ARG_3;
                    break;
                
                default:
                    s->state = BEGIN;
                    break;
            }
        break;
            
        case ARG_3:
            switch (terminal){
                
                case COMMA:
                    s->category = SEMI;
                    declareVar(s,line);
                    registerArgument(s,line);
                    s->state = ARG_1;
                    break;
                
                case CLOSE_ROUND:
                    s->category = SEMI;
                    declareVar(s,line);
                    registerArgument(s,line);
                    s->state = ARG_6;
                    break;

                case OPEN_SQUARE:
                    s->category = OPEN_SQUARE;
                    declareVar(s,line);
                    registerArgument(s,line);
                    s->state = ARG_4;
                    break;
                
                default:
                    s->state = BEGIN;
                    break;
            }
        break;

        case ARG_4:
            switch (terminal){
                case CLOSE_SQUARE:
                    s->state = ARG_5;
                    break;
                
                default:
                    s->state = BEGIN;
                    break;
                }
        break;

        case ARG_5:
            switch (terminal){
                case COMMA:
                    s->state = ARG_1;
                    break;

                case CLOSE_ROUND:
                    s->state = ARG_6;
                    break;
                
                default:
                    s->state = BEGIN;
                    break;
            }
        break;

        case ARG_6:
            s->state = BEGIN;
        break;
    }
}