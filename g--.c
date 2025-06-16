#include "parser.h"
#include "scanner.h"
#include "semantis.h"
#include "intermediary.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv){
    char option;
    int quiet_scanner = 1;
    int quiet_parser = 1;
    int quiet_semantis = 1;
    int quiet_intermediary = 1;

    while ((option = getopt (argc, argv, "lLpPsSiI")) != -1){

        switch (option){
            
            case 'L':
            case 'l':
                quiet_scanner = 0;
                break;
            
            case 'P':
            case 'p':
                quiet_parser = 0;
                break;
            
            case 'S':
            case 's':
                quiet_semantis = 0;
                break;

            case 'I':
            case 'i':
                quiet_intermediary = 0;
                break;
            
            case '?':
                printf("opção inválida -%c",option);
                return 1;
            
            default:
                break;
        }
    }
    
    scanner * scan = createScanner(argv[argc-1],quiet_scanner); 
    semantis * tix = createSemantis(26 * 26,quiet_semantis);
    parser * par = createParser(scan,tix,quiet_parser);
    
    parse(par);
    int success = scan->success && par->success && tix->success;

    if(success){
        intermediary * inter = createIntermediary(par,quiet_intermediary);
        generate(inter,inter->root);
        destroyIntermediary(inter);
    }

    destroyParser(par);
    return !success;
}