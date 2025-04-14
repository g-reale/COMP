#include "intermediary.h"

#include <stdlib.h>
#include <stdio.h>

intermediary * createIntermediary(parser * par, int quiet){
    intermediary * inter = malloc(sizeof(intermediary));

    intermediary aux ={
        .pseudo_asm = (quadruple*)malloc(0),
        .code_size = 0,
        .root = par->root,
        .anonymous_index = 0,
        .anonymous = (char*)malloc(sizeof(char) * 3),
        .quiet = quiet
    };

    *inter = aux;
    strcpy(inter->anonymous,"t_");
    return inter;
}

void destroyIntermediary(intermediary * inter){
    for(size_t i = 0; i < inter->code_size; i++)
        destroyQuadruple(inter->pseudo_asm[i]);
    free(inter->pseudo_asm);
    free(inter->anonymous);
    free(inter);
}

void insertQuadruple(intermediary * inter, quadruple quad){
    inter->pseudo_asm = realloc(inter->pseudo_asm, sizeof(quadruple) * (inter->code_size + 1));
    inter->pseudo_asm[inter->code_size] = quad;
    inter->code_size++;
}

void generate(intermediary * inter, node * root){

    // printf("%s\n",STATE_NAMES[root->token-START]);
    switch (root->token){
        
        case DECLARATION:{
            char * identifier = root->decedents[1]->lexeme;
            dfa_states instruction = VOID_FUN_DECL;
            if(root->decedents[2]->token == DECLARATION_DECISION)
                instruction = root->decedents[2]->decedents[0]->token;
            insertQuadruple(inter,createQuadruple(instruction,identifier,NULL,NULL,inter->quiet));
        }break;

        case PARAM:{
            char * identifier = root->decedents[1]->lexeme;
            dfa_states instruction = root->decedents[2]->decedents[0]->token;
            insertQuadruple(inter,createQuadruple(instruction,identifier,NULL,NULL,inter->quiet));
        }break;
            
        default:
            break;
    }

    for(size_t i = 0; i < root->decedent_amount; i++)
        generate(inter,root->decedents[i]);

    switch (root->token){
        
        case VOID_FUN_DECL:
        case INT_FUN_DECL:
            insertQuadruple(inter,createQuadruple(root->token,NULL,NULL,NULL,inter->quiet)); 
            break;
        
        default:
            break;
    }
}