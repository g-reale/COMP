#include "dgen.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//the base formating for the rules if {<sources>^<destinations>^<transitions>}
//if you want to make your life easier use shell enviroment variables to store things like numbers and letters
//that way you can do something like {$(LETTER)^0^$(NUMBER)} where LETTER:=abcdefg... and NUMBER:=0123..

int main(int argc, char * argv[]){
    
    memset(DFA,(unsigned char)'°', 256 * 256 * sizeof(unsigned char));

    if(argc < 3){
        fprintf(stderr,"not enought arguments, (%d)\n", argc);
        exit(0);
    }

    for(size_t i = 2; i < (size_t)argc; i++){
        size_t j = 0;
        while(argv[i][j]){
            printf("%c",argv[i][j]);
            scan((unsigned char)argv[i][j]);
            j++;
        }
        printf("\n");
    }
    exportDfa(argv[1]);
}