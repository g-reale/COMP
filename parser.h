/**
 * @file parser.h
 * @author Gabriel Reale  (gabriel.reale@unifesp.br)
 * @brief Mixed recursive descent and LL(1) parser
 * This file defines the prototypes for the parser abstraction.
 * The parser checks is the program in in the C- grammar by using a production table
 * for each token produced by the scanner, and non terminal of the grammar this tables tells
 * what production should be derived like in a LL(1) parser. Unlike a LL(1) parser, productions
 * can have EBNF blocks, and are parsed recursively without a manually managed stack.
 * - When a token is mismatched a error message is printed and the parser continues to the next token of the input.
 * - When a valid token is matched it is feed to the semantic analyzer (Only valid tokens are feed to the semantis).
 * - The grammar performs type checking of return statements for void functions, the whole grammar is defined in `ggenerate/ggen.py`. `ggen.py` is responsible 
 * for generating the production table and the firsts of the non terminals and exporting then to a .h header (`ggenerate/production_table.h`) 
 * 
 * @version 0.1
 * @date 2025-02-02
 */

#ifndef PARSER_H
#define PARSER_H

#include "semantis.h"
#include "scanner.h"
#include "stdint.h"

typedef struct node node;
/**
 * @brief Struct that defines a node of the syntax tree
 */
typedef struct node {
    /**
     * @brief Node token, can be both a terminal or non terminal
     */
    dfa_states token;
    /**
     * @brief Lexeme associated with the node if it is a terminal 
     * of type NUM or IDENTIFIER
     */
    char * lexeme;
    /**
     * @brief Descendants of the node. A node can have a arbitrary 
     * number of descendants (N-ary tree)  
     */
    node ** decedents;
    /**
     * @brief Amount of descendants connected to this node
     */
    size_t decedent_amount;
} node;

/**
 * @brief Structs that keeps the state of a parser
 */
typedef struct{
    /**
     * @brief Scanner to tokenize the program. The parser gradually makes calls
     * to @ref getToken when deriving the program
     */
    scanner * scan;
    /**
     * @brief Root of the syntax tree
     */
    node * root;
    /**
     * @brief Semantic analyzer @ref semantis. When valid tokens are parsed
     * the parser gives then to the semantic analyzer to create the symbol table and detect semantical errors
     */
    semantis * tix;
    /**
     * @brief If success = 1 at the end of the parse call the the program
     * was syntactically correct
     */
    int success;
    /**
     * @brief If 0 the parser will log it's information when destroyed.
     */
    int quiet;    
} parser;

/**
 * @brief Inserts a decedent into a node
 * 
 * @param root node to insert decedent to
 * @param token decedent's token
 * @param lexeme decedent's lexeme
 * @return node* newly created descendent
 */
node * createDecedent(node * root ,dfa_states token, char * lexeme);
/**
 * @brief Initializes a node
 * 
 * @param token node's token 
 * @param lexeme node's lexeme
 * @return node* newly created node
 */
node * createNode(dfa_states token, char * lexeme);
/**
 * @brief Recursively deallocates a syntax tree
 *
 * @param root root of the tree
 */
void destroyTree(node * root);
/**
 * @brief Traverses a tree printing relevant information
 * 
 * @param root root of the tree
 * @param depth createing depth value 
 */
void traverseTree(node * root, size_t * depth);
/**
 * @brief Starts a parser object
 * 
 * @param s scanner to bind to
 * @param tix semantic analyzer to bind to
 * @param quiet if 0 print relevant information before the parser's destruction
 * @return parser* initialized parser
 */
parser * createParser(scanner * s, semantis * tix, int quiet);
/**
 * @brief Destroys a parser object
 * 
 * @param p parser to destroy
 */
void destroyParser(parser * p);
/**
 * @brief Parser main function. When called:
 * - uses the scanner to tokenize the program
 * - uses the production table to parse the program
 * - uses the semantic analyzer to construct the symbol table
 * All of that happens in a single pass of the compiler.
 * @param p parser
 * @return int status (success or failure)
 */
int parse(parser * p);

#endif