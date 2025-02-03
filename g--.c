#include "parser.h"
#include "scanner.h"
#include "semantis.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv){
    char option;
    int quiet_scanner = 1;
    int quiet_parser = 1;
    int quiet_semantis = 1;

    while ((option = getopt (argc, argv, "lLpPsS")) != -1){

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
            
            case '?':
                printf("opção inválida -%c",option);
                return 1;
            
            default:
                break;
        }
    }
    
    scanner * scan = startScanner(argv[argc-1],quiet_scanner); 
    semantis * tix = startSemantis(26 * 26,quiet_semantis);
    parser * par = startParser(scan,tix,quiet_parser);
    
    parse(par);
    int success = scan->success && par->success && tix->success;
    destroyParser(par);
    return !success;
}