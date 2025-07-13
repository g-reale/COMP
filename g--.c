#include "parser.h"
#include "scanner.h"
#include "semantis.h"
#include "assembler.h"
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
    int quiet_assembler = 1;

    if(argc < 3){
        fprintf(stderr,"ERRO: não foram fornecidos argumentos o suficiente\n");
        return 1;
    }

    while ((option = getopt (argc, argv, "lLpPsSiIaA")) != -1){

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

            case 'A':
            case 'a':
                quiet_assembler = 0;
                break;
            
            case '?':
            fprintf(stderr,"ERRO: opção inválida -%c\n",option);
                return 1;
            
            default:
                break;
        }
    }
    
    scanner * scan = createScanner(argv[argc-2],quiet_scanner); 
    semantis * tix = createSemantis(26 * 26,quiet_semantis);
    parser * par = createParser(scan,tix,quiet_parser);
    
    parse(par);
    int success = scan->success && par->success && tix->success;
    
    if(!success){
        destroyParser(par);
        return 1;
    }

    intermediary * inter = createIntermediary(par,quiet_intermediary);
    assembler ass = createAssembler(argv[argc-1],inter,quiet_assembler);
    assemble(&ass);
    destroyAssembler(&ass);
    return 0;
}