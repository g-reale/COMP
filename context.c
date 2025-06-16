#include "context.h"
#include "dgenerate/states_dfa.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

context * createContext(size_t size, char * name, int explicit, int quiet){

    context * c = (context*)malloc(sizeof(context));
    c->name = (char*)malloc((strlen(name) + 1) * sizeof(char));
    strcpy(c->name,name);
    c->size = size;
    c->returned = 0;
    c->explicit = explicit;
    c->quiet = quiet;

    c->variables = (variable **) malloc(sizeof(variable*) * size);
    c->var_count = (size_t*)malloc(sizeof(size_t) * size);
    memset(c->var_count,0,sizeof(size) * size);

    return c;
}

void destroyContext(context * c){
    
    if(!c->quiet)
        printContext(c);

    //free the hash
    for(size_t i = 0; i < c->size; i++)
        destroyList(c->variables[i],c->var_count[i]);
    free(c->variables);
    free(c->var_count);
    
    //free the misc fields
    free(c->name);
    free(c);
}

void printContext(context * c){
    variable * list;
    size_t size;
    for(size_t i = 0; i < c->size; i++){
        if(c->var_count[i]){
            size = c->var_count[i];
            list = c->variables[i];
            
            for(size_t j = 0; j < size; j++){
                variable var = list[j];
                printf("%s;%s;%s;%s",c->name,var.lexeme,STATE_NAMES[var.type - START],STATE_NAMES[var.category - START]);

                for(size_t k = 0; k < var.use_amount; k++)
                    printf(";%ld",var.uses[k]);
                printf("\n");
            }
        }
    }
}


void destroyList(variable * list, size_t size){
    for(size_t i = 0; i < size; i++){
        free(list[i].lexeme);
        free(list[i].uses);
    }
    if(size)
        free(list);
}

size_t hash(char * lexeme, size_t size){
    
    // printf("hashing!\n");

    size_t i = 0;
    size_t index = 0;
    size_t step = 1;

    while(lexeme[i] != '\0'){
        index += lexeme[i] * step;
        step *= 26;
        // printf("%c %ld %ld\n",lexeme[i],i,index);
        i++;
    }

    return index % size;
}


variable * insert(variable ** list, size_t * size, dfa_states type, char * lexeme, dfa_states category, size_t line){
    
    if(!(*size)){
        *list = (variable*)malloc(0);
    }

    //create the new variable
    variable v;    
    v.lexeme = (char*)malloc(sizeof(char) * (strlen(lexeme) + 1));
    strcpy(v.lexeme,lexeme);
    v.use_amount = 1;
    v.uses = (size_t*)malloc(sizeof(size_t));
    v.uses[0] = line;
    v.type = type;
    v.category = category;

    //insert into list
    *list = (variable*)realloc(*list,sizeof(variable) * (*size + 1));
    *list[*size] = v;
    // printf("inserted at %ld %s\n",*size,(*list[*size]).lexeme);
    (*size)++;
    return *list + (*size-1);
}

variable * isUnique(variable * list, size_t size, char * lexeme){
    // printf("checking for uniqueness of %s on a list of size %ld\n",lexeme,size);
    for(size_t i = 0; i < size; i++){
        if(strcmp(list[i].lexeme,lexeme) == 0){
            // printf("sucess!\n");
            return &list[i];
        }
    }
    // printf("failure!\n");
    return NULL;
}

void useVar(variable * var, size_t line){
    var->uses = (size_t *)realloc(var->uses,sizeof(size_t) * (var->use_amount + 1));
    var->uses[var->use_amount] = line;
    var->use_amount += 1;
}


