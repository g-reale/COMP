/**
 * @file scanner.h
 * @author Gabriel Reale - Stefanini Rafael (groliveira5@stefanini.com)
 * @brief DFA based scanner.
 * This file defines the prototypes for the scanner abstraction.
 * The scanner must be able to tokenize a program  stored in a @ref buffer via the @ref getToken call.
 * Currently the scanner uses a dfa table for categorizing the lexemes where the ambiguity between keywords and 
 * identifiers is resolved using a hash.
 * @version 0.1
 * @date 2025-01-04
 */

#ifndef SCANNER_H
#define SCANNER_H

#include "dgenerate/states_dfa.h"
#include "buffer.h"

/**
 * @brief Struct that keeps the state of a scanner
 */
typedef struct{
    /**
     * @brief Buffer with the program being tokenized
     */
    buffer * program;
    /**
     * @brief Set to 1 when done tokenizing
     */
    int done;
}scanner;

/**
 * @brief Starts a scanner struct
 * @param path path pointing to the program
 * @return scanner* initialized scanner
 */
scanner * startScanner(char * path);
/**
 * @brief Destroys a scanner
 * @param s scanner to destroy<br>
 * Will free( @ref s ) <br>
 * Will call @ref destroyBuffer
 */
void destroyScanner(scanner * s);
/**
 * @brief Get the next token of the program.
 * @param s program scanner
 * @param lexeme lexeme string
 * @return dfa_states returned token
 */
dfa_states getToken(scanner * s, char ** lexeme);
#endif