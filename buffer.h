/**
 * @file buffer.h
 * @author Gabriel Reale - Unifesp (gabrie.reale@unifesp.br)
 * @brief Dynamic char buffer.
 * This file defines the prototypes for the buffer abstraction. 
 * The buffer's main responsibility is the direct interaction with the program file and returning chars when requested.  
 * The returned chars feed the @ref scanner.h abstraction that can subsequently return the tokenized program.
 * @version 0.1
 * @date 2025-01-04 
 */

#ifndef BUFFER_H
#define BUFFER_H

#include <stdio.h>
#include <stddef.h>

/**
 * @brief Struct that keeps the state of the buffer
 */
typedef struct{
    /**
     * @brief Current line in the file
     */
    size_t line;
    /**
     * @brief Current column of the line
     */
    size_t column;
    /**
     * @brief Previous column (will update the column variable in case of a @ref ungetChar call, but only once)
     */
    size_t prev_column;
    /**
     * @brief Current char to return
     */
    size_t at;
    /**
     * @brief Chunk size
     */
    size_t size;
    /**
     * @brief Amount of samples to keep from the last chunk (sets maximum number of chained safe @ref ungetChar calls).
     * Is fixed to 1 i.e always keep 1 old char when loading a new chunk.
     */
    size_t halo;
    /**
     * @brief Set to 1 when the file content has ended
     */
    int done;
    /**
     * @brief File to read
     */
    FILE * fid;
    /**
     * @brief File chunk
     */
    char * chunk;
} buffer;

/**
 * @brief Starts a buffer struct
 * @param path file to read contents from (if the path is invalid the program exits with a error)
 * @param size  internal chunk size (if the size is smaller than the @ref buffer::halo the program exits with a error)
 * @return buffer* allocated buffer
 */
buffer * startBuffer(char * path,size_t size);
/**
 * @brief Get the currently pointed char.<br>
 * Will @ref buffer::chunk chars from the file if needed.<br>
 * Will increment the @ref buffer::at variable.<br>
 * Will keep track of the current @ref buffer::line of the file.<br>
 * Will set @ref buffer::done to 1 if the returned char is EOF
 * @param b buffer struct
 * @return char 
 */
char getChar(buffer * b);
/**
 * @brief Copy the last size characters in relation to the @ref buffer::at variable (if @ref buffer::at < size the program exits with a error)<br>
 * Will append a '\0' to the end of the @ref copy.
 * @param b buffer struct
 * @param copy copied string
 * @param size amount of chars to copy
 */
void copyLast(buffer * b, char ** copy, size_t size);
/**
 * @brief Go back to the previous buffer state<br>
 * Will decrement @ref buffer::at if 0 < @ref buffer::at.<br>
 * Will decrement the @ref buffer::line number if the backtracked char was a line end.
 * @param b 
 */
void ungetChar(buffer * b);
/**
 * @brief Destroys a buffer struct<br>
 * Will free the @ref buffer::chunk vector.<br>
 * Will close the file on @ref buffer::fid.
 * @param b buffer struct
 */
void destroyBuffer(buffer * b);
#endif