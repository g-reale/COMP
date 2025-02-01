#include "parser.h"
#include "scanner.h"
#include <stdio.h>
#include <stdlib.h>

int main(){
    scanner * scan = startScanner("test1.txt"); 
    parser * par = startParser(scan);
    parse(par);
    // char * lexeme = malloc(0);
    
    //crude test rig
    // while(!scan->done){
    //     dfa_states tok = getToken(scan,&lexeme);
    //     printf("%ld: %s %s\n",scan->program->line,STATE_NAMES[tok - START]);
    // }
    // free(lexeme);    
    // destroyScanner(scan);
    destroyParser(par);
    return 1;
}