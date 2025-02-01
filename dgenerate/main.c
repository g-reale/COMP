#include "dgen.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// usage <destination_path><rules>

int main(int argc, char * argv[]){
    
    memset(DFA,(unsigned char)34, 256 * 256 * sizeof(unsigned char));

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