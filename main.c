#include "scanner.h"
#include <stdio.h>
#include <stdlib.h>

int main(){
    scanner * scan = startScanner("test.txt"); 
    
    char * lexeme = malloc(0);
    
    //crude test rig
    while(!scan->done){
        char c = (unsigned char)getToken(scan,&lexeme);
        printf("%ld: ",scan->program->line);
        switch (c) {
        case START:
            printf("START\n");
            break;

        case END:
            printf("END\n");
            break;

        case FAILURE:
            printf("FAILURE\n");
            break;

        case SUM:
            printf("SUM\n");
            break;

        case SUB:
            printf("SUB\n");
            break;

        case MUL:
            printf("MUL\n");
            break;

        case DIV:
            printf("DIV\n");
            break;

        case LESS:
            printf("LESS\n");
            break;

        case MORE:
            printf("MORE\n");
            break;

        case SEMI:
            printf("SEMI\n");
            break;

        case COMMA:
            printf("COMMA\n");
            break;

        case EQUAL:
            printf("EQUAL\n");
            break;

        case OPEN_ROUND:
            printf("OPEN_ROUND\n");
            break;

        case CLOSE_ROUND:
            printf("CLOSE_ROUND\n");
            break;

        case OPEN_SQUARE:
            printf("OPEN_SQUARE\n");
            break;

        case CLOSE_SQUARE:
            printf("CLOSE_SQUARE\n");
            break;

        case OPEN_CURLY:
            printf("OPEN_CURLY\n");
            break;

        case CLOSE_CURLY:
            printf("CLOSE_CURLY\n");
            break;

        case LESS_EQ:
            printf("LESS_EQ\n");
            break;

        case MORE_EQ:
            printf("MORE_EQ\n");
            break;

        case LOGICAL_EQ:
            printf("LOGICAL_EQ\n");
            break;

        case NOT:
            printf("NOT\n");
            break;

        case NOT_EQUAL:
            printf("NOT_EQUAL\n");
            break;

        case NUM:
            printf("NUM: %s\n",lexeme);
            break;

        case IDENTIFIER:
            printf("IDENTIFIER: %s\n",lexeme);
            break;

        case AMBIGUOUS:
            printf("AMBIGUOUS\n");
            break;

        case COMMENT:
            printf("COMMENT\n");
            break;

        case COMMENT_OUT:
            printf("COMMENT_OUT\n");
            break;

        case IF:
            printf("IF\n");
            break;

        case INT:
            printf("INT\n");
            break;

        case VOID:
            printf("VOID\n");
            break;

        case ELSE:
            printf("ELSE\n");
            break;

        case WHILE:
            printf("WHILE\n");
            break;

        case RETURN:
            printf("RETURN\n");
            break;

        default:
            printf("OTHER: %c\n", c);
            break;
    }}
    free(lexeme);    
    destroyScanner(scan);
    return 1;
}