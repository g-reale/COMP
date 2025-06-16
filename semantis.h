/**
 * @file semantis.h
 * @author Gabriel Reale  (gabriel.reale@unifesp.br)
 * @brief Semantic analysis i.e semantis
 * This file defines the prototypes for the semantic analyzer abstraction.
 * The analyzer uses a DFA to decide what semantic action should be taken for each token of the input.
 * When the analyzer's DFA detects a new context, it is immediately stacked, by the same coin, when a 
 * context exits the stack is popped. When a context is popped it's symbol table is done, and can be printed to the 
 * stdout if the semantis is not quiet.
 * 
 * Currently the semantis detects:
 * - symbol use before declaration
 * - symbol redeclaration
 * - int function with no return calls (control flow)
 * - main not declared
 * - main does not returns void
 * - function declared after main
 * 
 * @version 0.1
 * @date 2025-02-02
 */

#ifndef semantis_H
#define semantis_H

#include "context.h"
#include "dgenerate/states_dfa.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * @brief States of the analyzer DFA
 */
typedef enum{
    BEGIN,
    DECL_1,
    DECL_2,
    USE_1,
}semantis_states;


/**
 * @brief Struct that defines a semantic analyzer
 */
typedef struct {
    /**
     * @brief Context stack
     */
    context ** context_stack;
    /**
     * @brief Size of the hash of each context
     */
    size_t context_size;
    /**
     * @brief Index of the stack top
     */
    size_t top;
    
    /**
     * @brief Internal register used to keep the type
     * of the currently analyzed variable
     */
    dfa_states type;
    /**
     * @brief Internal register used to keep the lexeme
     * of the currently analyzed variable
     */
    char * lexeme;
    /**
     * @brief Internal register that keeps the category
     * of the currently analyzed variable (`OPEN_ROUND` = function, `SEMI` = scalar, `OPEN_SQUARE` = vector)
     */
    dfa_states category;
    /**
     * @brief Pointer to the currently analyzed function
     */
    variable * func;
    /**
     * @brief Argument count of the currently analyzed function
     */
    size_t arg_count;

    /**
     * @brief State of the semantis's DFA
     */
    semantis_states state;

    /**
     * @brief If set to 1 the main function has already been declared
     */
    int main_declared;
    /**
     * @brief If set to 1 at the end o the analysis the semantic of the program is correct
     */
    int success;
    /**
     * @brief If set to 1 will print the symbol table of the context when it is popped
     */
    int quiet;
}semantis;


/**
 * @brief Starts a semantix object
 * 
 * @param context_size size of the hash to use when creating a context
 * @param quiet if 0 print relevant information when a context is popped
 * @return semantis* initialized semantis
 */
semantis * createSemantis(size_t context_size, int quiet);
/**
 * @brief Destroys a semantis object
 * 
 * @param s semantis do detroy
 */
void destroySemantis(semantis * s);
/**
 * @brief Stacks a context
 * 
 * @param s semantis where the action should be taken
 * @param anonymous if set to 1 the context is anonymous i.e it is not 
 * named by the user (function), and it's name shall be derived from the current
 * stack top name + '_'
 */
void stackContext(semantis * s, int explicit, int anonymous);
/**
 * @brief Pops a context. If the popped context has to be explicitly returned, print
 * a error if the return flag was not one. 
 * Also set the new stack top return flag to the popped context return flag (i.e functions can be returned 
 * from nested contexts).
 * 
 * @param s semantis where the action should be taken
 * @param line current line being analyzed
 */
void popContext(semantis * s, size_t line);
/**
 * @brief Inserts a variable on the current context. The type, name and category 
 * of the variable are taken from the `s->type`, `s->lexeme` and `s->category` registers.
 * - If the variable has already been declared in the current context print a error.
 * - If the variable was the main function, set the `main_declared_flag` to one
 * - If the variable was the main function, and it's type was not `VOID` print a error
 * - If the variable was a void function stack a context
 * - If the variable was a int function stack a explicit context
 * @param s semantis where the action should be taken
 * @param line current line being analyzed
 * @return variable* reference to the newly declared variable, if declaration was unsuccessful is set to `NULL`
 */
variable * declareVar(semantis * s, size_t line);
/**
 * @brief Searches for a variable name on the context stack.
 * - If no variable was found, prints a error saying that the variable was used before declaration adn returns `NULL`.
 * - If a variable was found and no errors were produced, call @ref useVar at the current line
 * 
 * @param s semantis where the action should be taken
 * @param line current line being analyzed
 * @return variable* reference to the found variable, if search was unsuccessful return NULL 
 */
variable * searchVar(semantis * s, size_t line);
/**
 * @brief Main function of the semantic analyzer. Execute a DFA using the input tokens 
 * and execute semantic actions as the `s->state` variable changes
 * 
 * @param s semantis where the action should be taken
 * @param terminal token/terminal being analyzed
 * @param lexeme token's lexeme
 * @param line line being analyzed
 */
void analise(semantis * s, dfa_states terminal, char * lexeme, size_t line);

#endif