/**
 * @file context.h
 * @author Gabriel Reale  (gabriel.reale@unifesp.br)
 * @brief This file declares support abstractions for the semantic analyzer @ref semantis.
 * The idea is to define what is a variable and what is a context and what they can do before producing the symbol table.
 * @version 0.1
 * @date 2025-02-02
 */

#ifndef CONTEXT_H
#define CONTEXT_H

#include "dgenerate/states_dfa.h" 

#include <stddef.h>

/**
 * @brief Struct that define what is a variable
 */
typedef struct variable{
    /**
     * @brief Variable type ( INT | VOID )
     */
    dfa_states type;
    /**
     * @brief Variable lexeme/name
     */
    char * lexeme;
    /**
     * @brief Variable category (SEMI = scalar | OPEN_SQUARE = vector | OPEN_ROUND = function)
     */
    dfa_states category;
    /**
     * @brief Lines where this variable is used
     */
    size_t * uses;
    /**
     * @brief Amount of uses of this variable
     */
    size_t use_amount;
}variable;

/**
 * @brief Struct that defines what is a context
 */
typedef struct{
    /**
     * @brief Context name
     */
    char * name;
    /**
     * @brief Amount of buckets to keep int this context's hash
     */
    size_t size;
    /**
     * @brief Amount of variables at each bucket
     */
    size_t * var_count;
    /**
     * @brief Variable hash / bucket list
     */
    variable ** variables;
    /**
     * @brief If any return keyword happened in this context
     */
    int returned; 
    /**
     * @brief If this context explicitly needs at least one return statement i.e
     * this is a int function  
     */
    int explicit;
    /**
     * @brief If set to 0 will print relevant information when this context is destroyed
     */
    int quiet;
}context;

/**
 * @brief Starts a context struct
 * 
 * @param size hash size
 * @param name context name
 * @param quiet if set to 0 will print relevant information when destroyed
 * @return context* Newly created context
 */
context * createContext(size_t size, char * name, int explicit, int quiet);
/**
 * @brief Destroys a context
 * 
 * @param c context to destroy
 */
void destroyContext(context * c);
/**
 * @brief Prints the relevant information of a context
 * 
 * @param c context to display
 */
void printContext(context * c);
/**
 * @brief Destroy a bucket of variables
 * 
 * @param list bucket to destroy
 * @param size size of the bucket
 */
void destroyList(variable * list, size_t size);
/**
 * @brief Hash a lexeme
 * 
 * @param lexeme lexeme to hash
 * @param size size of the hash table
 * @return size_t index of the bucket
 */
size_t hash(char * lexeme, size_t size);
/**
 * @brief Inserts a variable inside a bucket
 * 
 * @param list bucket to insert the variable into
 * @param size size of the bucket
 * @param type type of the variable
 * @param lexeme lexeme of the variable
 * @param category category of the variable
 * @param line line where the variable was declared
 * @return variable* newly created variable
 */
variable * insert(variable ** list, size_t * size, dfa_states type, char * lexeme, dfa_states category, size_t line);
/**
 * @brief Check if a variable name is unique within a bucket
 * 
 * @param list bucket to search variable
 * @param size size of the bucket
 * @param lexeme variable lexeme 
 * @return variable* if NULL no variable was found i.e is unique, else 
 * returns a reference to the variable
 */
variable * isUnique(variable * list, size_t size, char * lexeme);
/**
 * @brief Ads a new line into the variable use list
 * 
 * @param var used variable
 * @param line line of the use
 */
void useVar(variable * var, size_t line);
#endif