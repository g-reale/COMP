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

        case EXPRESSION:{
            if(root->decedent_amount == 1){
                root->lexeme = strdup(root->decedents[0]->lexeme);
                return;
            }

            for(size_t i = root->decedent_amount -1; i - 1 < i; i--){
                if(root->decedents[i]->token != EQUAL)
                    continue;
                insertQuadruple(inter,createQuadruple(EQUAL,root->decedents[i-1]->lexeme,root->decedents[i+1]->lexeme,NULL,inter->quiet));
            }

        }break;

        case SIMPLE_EXP:
        case TERM:
        case SUM_EXP:{

            if(root->decedent_amount == 1){
                root->lexeme = strdup(root->decedents[0]->lexeme);
                return;
            }

            char * source_A = strdup(root->decedents[0]->lexeme);
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

                source_B = root->decedents[i]->lexeme;
                insertQuadruple(inter,createQuadruple(instruction,inter->anonymous,source_A,source_B,inter->quiet));
                free(source_A);
                source_A = strdup(inter->anonymous);
                if(i+1 < root->decedent_amount) nextLabel(inter);
            }
            free(source_A);
            root->lexeme = (char*)malloc(sizeof(char) * strlen(inter->anonymous)+1);
            strcpy(root->lexeme,inter->anonymous);
            nextLabel(inter);
            
        }break;
        
        case VOID_FUN_DECL:
        case INT_FUN_DECL:{
            insertQuadruple(inter,createQuadruple(root->token,NULL,NULL,NULL,inter->quiet)); 
        }break;

        case FACTOR:{
            
            //just identifier or number
            if(root->decedent_amount == 1){
                root->lexeme = strdup(root->decedents[0]->lexeme);
                return;
            }

            //expression
            if(root->decedents[0]->token == OPEN_ROUND){
                root->lexeme = strdup(root->decedents[1]->lexeme);
                return;
            }

            //vector activation
            if(root->decedents[1]->decedents[0]->token == VECTOR_ACTIVATION){
                insertQuadruple(inter,createQuadruple(
                                    MUL,
                                    inter->anonymous,
                                    root->decedents[1]->decedents[0]->decedents[1]->lexeme,
                                    "8",
                                    inter->quiet
                                ));
                char * source_B = strdup(inter->anonymous);
                nextLabel(inter);
                
                insertQuadruple(inter,createQuadruple(
                    VECTOR_ACTIVATION,
                    inter->anonymous,
                    root->decedents[0]->lexeme,
                    source_B,
                    inter->quiet    
                ));
                root->lexeme = strdup(inter->anonymous);
                nextLabel(inter);
                free(source_B);
                return;
            }

            //function activation
            node * arguments = root->decedents[1]->decedents[0]->decedents[1];
            for(size_t i = 0; i < arguments->decedent_amount; i++){
                if(arguments->decedents[i]->token != ARGUMENT)
                    continue;

                insertQuadruple(inter,createQuadruple(
                    ARGUMENT,
                    arguments->decedents[i]->decedents[0]->lexeme,
                    NULL,
                    NULL,
                    inter->quiet
                ));
            }

            insertQuadruple(inter,createQuadruple(
                FUNCTION_ACTIVATION,
                inter->anonymous,
                root->decedents[0]->lexeme,
                NULL,
                inter->quiet
            ));

            root->lexeme = strdup(inter->anonymous);
            nextLabel(inter);
            return;

        }break;

        default:
            break;
    }
}