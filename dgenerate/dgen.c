#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dgen.h"

unsigned char DFA[256][256] = {};

void createTransition(transition t){
    
    size_t i = 0;
    size_t j = 0; 
    
    if(!t.sources || !t.destination)
        return;

    while(t.sources[i]){
        while(t.transitions[j]){
            size_t source = t.sources[i];
            size_t transition = t.transitions[j];
            DFA[source][transition] = t.destination;
            j++;
        }
        j = 0;
        i++;
    }
}

void createReverseTransition(transition t){
    
    size_t i = 2;
    size_t j = 0; 
    
    if(!t.sources || !t.destination)
        return;

    while(t.transitions[i]){
        for(size_t transition = 0; transition < 256; transition++){
            if(t.transitions[i] == (unsigned char)transition)
                continue;
            
            j = 0;
            while(t.sources[j]){
                size_t source = t.sources[j];
                DFA[source][transition] = t.destination;
                j++;
            }
        }
        i++;
    }
}

void cretateAllTransitions(transition t){
    
    size_t i = 0;
    if(!t.destination)
        return;    
    
    while(t.sources[i]){
        for(size_t transition = 0; transition < 256; transition++){
            size_t source = t.sources[i];
            DFA[source][transition] = t.destination;
        }
        i++;
    }
}

void exportDfa(const char * path){
    
    FILE * file;
    file = fopen(path,"w");
    if(file == NULL){
        fprintf(stderr,"error opening file!\n");
        exit(1);
    }
    
    fprintf(file,"#ifndef DFA_H\n");
    fprintf(file,"#define DFA_H\n");
    fprintf(file,"unsigned char DFA[256][256] = {");

    for(size_t i = 0; i < 256; i++){
        fprintf(file,"{");
        
        for(size_t j = 0; j < 256;j++){
            fprintf(file,"%d",DFA[i][j]);
            if(j != 255)
                fprintf(file,", ");
        }

        if(i != 255)
            fprintf(file,"},\n");

    }

    fprintf(file,"}};\n");
    fprintf(file,"#endif");
}



void categorize(transition t){
    
    printf(" _%s_%c_%s_",t.sources,t.destination,t.transitions);
    size_t i = 0;
    while(t.transitions[i] && i < 2)
        i++;
    
    if(i < 2){
        printf("default");
        createTransition(t);
    }
    else{
        if(t.transitions[0] == '*' && t.transitions[1] == '*'){
            printf("all");
            cretateAllTransitions(t);
        }
        else if(t.transitions[0] == '-' && t.transitions[1] == '-'){
            printf("reverse");
            createReverseTransition(t);
        }
        else{
            printf("default");
            createTransition(t);
        }
    }
}

#define sep '_'
#define open '_'
#define close '_'

void scan(unsigned char c){

    typedef enum{
        START,
        SOURCES,
        DESTINATION,
        SEPARATOR,
        TRANSITIONS,
    }states;

    static states state = START;
    static size_t i = 0, j = 0;

    static unsigned char sources[257] = {'\0'};
    static unsigned char transitions[257] = {'\0'};
    static unsigned char destination = '\0';
    
    switch (state){
        
        case START:
            i = j = 0;
            state = c == open ? SOURCES : START;
        break;

        case SOURCES:
            switch (c){
                
                default:
                    state = i == 256 ? START : SOURCES;
                    sources[i] = c;
                    i++;
                break;

                case sep:
                    sources[i] = '\0';
                    state = DESTINATION;
                break;
            
            }
        break;

        case DESTINATION:
            destination = c;
            state = SEPARATOR;
        break;
        
        case SEPARATOR:
            state = c == sep ? TRANSITIONS : START;
        break;
        
        case TRANSITIONS:
            switch (c){

                default:
                    state = j == 256 ? START : TRANSITIONS;
                    transitions[j] = c;
                    j++;
                break;

                case close:
                    transitions[j] = '\0';
                    categorize((transition){
                        sources,
                        destination,
                        transitions
                    });
                    state = START;    
                break;
            }
        break;
    }
}