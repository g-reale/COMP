#include "ggenerate/production_table.h"
#include "dgenerate/states_dfa.h"
#include "semantis.h"
#include "scanner.h"
#include "parser.h"

#include <stdlib.h>
#include <string.h>

parser * createParser(scanner * s, semantis * tix, int quiet){
    parser * p = (parser*)malloc(sizeof(parser));
    p->root = createNode(PROGRAM,NULL);
    p->tix = tix;
    p->scan = s;
    p->success = 1;
    p->quiet = quiet;
    return p;
}

void destroyParser(parser * p){
    destroySemantis(p->tix);
    destroyScanner(p->scan);
    
    if(!p->quiet){
        size_t depth = 0;
        traverseTree(p->root,&depth);
    }
    
    destroyTree(p->root);
    free(p);
}

node * createNode(dfa_states token, char * lexeme){
    node * nd = (node*)malloc(sizeof(node));
    if(token == IDENTIFIER || token == NUM){
        nd->lexeme = (char*)malloc((strlen(lexeme) + 1) * sizeof(char));
        strcpy(nd->lexeme,lexeme);
    }
    else
        nd->lexeme = NULL;
    nd->token = token;
    nd->decedents = (node **)malloc(0);
    nd->decedent_amount = 0;
    return nd;
}

void destroyTree(node * root){
    for(size_t i = 0; i < root->decedent_amount; i++)
        destroyTree(root->decedents[i]);
    free(root->lexeme);
    free(root->decedents);
    free(root);
}

node * createDecedent(node * root ,dfa_states token, char * lexeme){
    root->decedent_amount++;
    root->decedents = (node**)realloc(root->decedents,sizeof(node*) * root->decedent_amount);
    root->decedents[root->decedent_amount-1] = createNode(token,lexeme);
    return root->decedents[root->decedent_amount-1];
}

void traverseTree(node * root, size_t * depth){
    char * ident = malloc((*depth+1) * sizeof(char));
    memset(ident,' ',*depth);
    ident[*depth] = '\0';

    printf("%s%s[%s]",ident,STATE_NAMES[root->token-START],root->lexeme);

    if(root->decedent_amount){
        printf("(\n");
        for(size_t i = 0; i < root->decedent_amount; i++){
            *depth = *depth + 1;
            traverseTree(root->decedents[i],depth);
            printf(",\n");
            *depth = *depth - 1;
        }
        printf("%s)",ident);
    }
    free(ident);
}

void parseRecursively(parser * p, node * root, dfa_states * production, size_t index, int on_EBNF){
    
    dfa_states symbol = production[index];
    dfa_states terminal = p->scan->token;

    while(symbol != END && symbol != FAILURE){

        //next production
        if(IS_NON_TERMINAL[symbol]){
            parseRecursively(p,
                             createDecedent(root,symbol,p->scan->lexeme),
                             PARSER[symbol][terminal],
                             0,
                             0);
            terminal = p->scan->token;
        }

        //try to match
        else if(symbol != OPTIONAL && symbol != REPETITION){
            if(terminal == symbol){
                analise(p->tix,terminal,p->scan->lexeme,p->scan->program->line);
                createDecedent(root,terminal,p->scan->lexeme);
            }
            else{
                fprintf(stderr,"ERRO SINTÁTICO: \"%s\" INVALIDO LINHA: %zu, COLUNA: %zu \n",STATE_NAMES[terminal-START],p->scan->program->line,p->scan->program->column);
                p->success = 0;
            }
            terminal = getToken(p->scan);
        }

        //open EBNF block
        else if(!on_EBNF){
            dfa_states command = symbol; 
            dfa_states next_symbol = production[index + 1];
            
            //try to match 
            while(IN_FIRST[next_symbol][terminal]){
                parseRecursively(p,
                                 root,
                                 production,
                                 index + 1,
                                 1);
                
                terminal = p->scan->token;
                if(command == OPTIONAL)
                    break;
            }
            
            //could not match optional block, advance util closure
            do{
                index++;
                symbol = production[index];
            }while(symbol != REPETITION && symbol != OPTIONAL);
        }
        
        //close EBNF block (no nested blocks allowed)
        else
            return;
        
        index++;
        symbol = production[index];   
    }
}

int parse(parser * p){
    
    //execute the fist iteration manually
    dfa_states non_terminal = PROGRAM;
    dfa_states terminal = getToken(p->scan);
    dfa_states * production = PARSER[non_terminal][terminal];
    
    //recursively parse the program
    parseRecursively(p,p->root,production,0,0);
    // size_t depth = 0;
    // traverseTree(p->root,&depth);
    return p->success;
}