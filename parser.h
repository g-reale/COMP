#include "scanner.h"
#include "stdint.h"


typedef struct node node;
typedef struct node {
    dfa_states token;
    char * lexeme;
    node ** decedents;
    size_t decedent_amount;
} node;

typedef struct{
    scanner * scan;
    node * root;
    int success;    
} parser;

node * createDecedent(node * root ,dfa_states token, char * lexeme);
node * createNode(dfa_states token, char * lexeme);
void destroyTree(node * root);

parser * startParser(scanner * s);
void destroyParser(parser * p);
int parse(parser * p);

void recursionMatch(parser * p, node * root, dfa_states * production);
int optionalMatch(parser * p, node * root, dfa_states ** production);