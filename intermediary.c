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
        .anonymous = (char*)malloc(sizeof(char) * 4),
        .quiet = quiet
    };

    *inter = aux;
    strcpy(inter->anonymous,"t_0");
    return inter;
}

void nextLabel(intermediary * inter){
    inter->anonymous_index++;
    size_t size = snprintf ( NULL, 0, "t_%ld", inter->anonymous_index);
    inter->anonymous = (char*)realloc(inter->anonymous,size+1);
    sprintf(inter->anonymous,"t_%ld",inter->anonymous_index);
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

        case FACTOR:
            char * identifier = root->decedents[0]->token == IDENTIFIER ||
                                root->decedents[0]->token == NUM ?
                                root->decedents[0]->lexeme:
                                root->decedents[1]->lexeme;
            root->lexeme = (char*)malloc(sizeof(char) * strlen(identifier)+1);
            strcpy(root->lexeme,identifier);
            break;

        case EXPRESSION:
            break;

        case SIMPLE_EXP:
        case TERM:
        case SUM_EXP:

            if(root->decedent_amount == 1){
                root->lexeme = (char*)malloc(sizeof(char) * strlen(root->decedents[0]->lexeme)+1);
                strcpy(root->lexeme,root->decedents[0]->lexeme);
                return;
            }

            char * source_A = root->decedents[0]->lexeme;
            char * source_B;
            dfa_states instruction;
            
            for(size_t i = 1; i < root->decedent_amount; i++){
                if(root->decedents[i]->token == MUL_DIV || 
                   root->decedents[i]->token == PLUS_MINUS ||
                   root->decedents[i]->token == RELATIONAL
                ){
                    instruction = root->decedents[i]->decedents[0]->token;
                    continue;
                }

                if(root->decedents[i]->token == EQUAL){
                    instruction = EQUAL;
                    continue;
                }
                
                source_B = root->decedents[i]->lexeme;
                insertQuadruple(inter,createQuadruple(instruction,inter->anonymous,source_A,source_B,inter->quiet));
                source_A = inter->anonymous;
                nextLabel(inter);
            }
            
            root->lexeme = (char*)malloc(sizeof(char) * strlen(inter->anonymous)+1);
            strcpy(root->lexeme,inter->anonymous);
            break;
        
        case VOID_FUN_DECL:
        case INT_FUN_DECL:
            insertQuadruple(inter,createQuadruple(root->token,NULL,NULL,NULL,inter->quiet)); 
            break;
        
        default:
            break;
    }
}