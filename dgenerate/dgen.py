#!/usr/bin/env python3
"""
 @file dgen.py
 @author Gabriel Reale - Stefanini Rafael (groliveira5@stefanini.com)
 @brief Configure dfa generation on a higher level. Rule Generation tool

 This python script serves as a config file of sorts. Here i can 
 create rules and invoke the dfa generator (@ref dgen.h) more easily, resulting in the file @ref dfa.h . Also i can export a enum with
 the generated dfa states based on the rules declared.
 
 The rules are defined on the @ref _ARGUMETS variable, each rule takes the format
 `_<SOURCES>_<DESTINATION>_<TRANSITION>_` (see: @ref dgen.h)<br>
 where:
 - `SOURCES`: must be a variable containing a list of chars
 - `DESTINATION`: must be a variable containing a single char
 - `TRANSITION`: must be a list of chars or a variable containing a list of chars
 
 Variables that start with `_` won't be exported on the resulting enum.
 Variables that don't start with `_` will be exported to @ref dfa_states. 
 The variable name will become a enum key and the variable value at the index[0] will become the enum value
 
 @version 0.1
 @date 2025-01-04
"""

import os

def configureDFA():
    """
    Function that performs the generation
    """    

    #auxiliar variables
    _LOWERCASE ="abcdefghijklmnopqrstuvwxyz"
    _UPPERCASE ="ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    _LETTERS = _LOWERCASE + _UPPERCASE 
    _NUMBERS ="0123456789"
    _ANY ="**"

    #start and end
    START="0"
    END ="?"
    FAILURE=':'

    # single character tokens
    SUM="1"
    SUB="2"
    MUL="3"
    DIV="4"
    LESS="5"
    MORE="6"
    SEMI="7"
    COMMA="8"
    EQUAL="a"
    OPEN_ROUND = "b"
    CLOSE_ROUND = "c"
    OPEN_SQUARE = "d"
    CLOSE_SQUARE = "e"
    OPEN_CURLY = "f"
    CLOSE_CURLY = "g"
    _SINGLE_CHARACTER_TOKENS = SUM + SUB + MUL + DIV + LESS + MORE + SEMI + COMMA + EQUAL + OPEN_ROUND + CLOSE_ROUND + OPEN_SQUARE + CLOSE_SQUARE + OPEN_CURLY + CLOSE_CURLY

    # double character tokens
    LESS_EQ = "h"
    MORE_EQ = "i"
    LOGICAL_EQ = "j"
    NOT = "k"
    NOT_EQUAL = "l"
    _DOUBLE_CHARACTER_TOKENS = LESS_EQ + MORE_EQ + LOGICAL_EQ + NOT_EQUAL

    # numbers
    NUM = "m"

    # ambiguous/identifiers/keywords
    IDENTIFIER = "n"
    AMBIGUOUS = "o"

    # comments 
    COMMENT = "p"
    COMMENT_OUT = "q"

    # keywords
    IF = "r"
    INT = "s"
    VOID = "t"
    ELSE = "u"
    WHILE = "v"
    RETURN = "w"

    _ARGUMENTS = [
        #output file name
        "dfa.h",
        
        # handle any non overwriten characters
        f"_{START}_{START}_{_ANY}_",

        # single character tokens (overwrite some of the START state transitions)
        f"_{START}_{SUM}_+_",
        f"_{START}_{SUB}_-_",
        f"_{START}_{MUL}_*_",
        f"_{START}_{DIV}_/_",
        f"_{START}_{LESS}_<_",
        f"_{START}_{MORE}_>_",
        f"_{START}_{SEMI}_;_",
        f"_{START}_{COMMA}_,_",
        f"_{START}_{EQUAL}_=_",
        f"_{START}_{OPEN_ROUND}_(_",
        f"_{START}_{CLOSE_ROUND}_)_",
        f"_{START}_{OPEN_SQUARE}_[_",
        f"_{START}_{CLOSE_SQUARE}_]_",
        f"_{START}_{OPEN_CURLY}_{{_",
        f"_{START}_{CLOSE_CURLY}_}}_",
        f"_{_SINGLE_CHARACTER_TOKENS}_{END}_{_ANY}_",

        # double character tokens
        f"_{LESS}_{LESS_EQ}_=_",
        f"_{MORE}_{MORE_EQ}_=_",
        f"_{EQUAL}_{LOGICAL_EQ}_=_",
        f"_{START}_{NOT}_!_",
        f"_{NOT}_{NOT_EQUAL}_=_",
        f"_{_DOUBLE_CHARACTER_TOKENS}_{END}_{_ANY}_",

        # numbers 
        f"_{START}_{NUM}_{_NUMBERS}_",
        f"_{NUM}_{END}_{_ANY}_",
        f"_{NUM}_{NUM}_{_NUMBERS}_", #overwrite the above any transition for numbers

        # identifiers
        f"_{START}_{IDENTIFIER}_{_UPPERCASE}_",
        f"_{IDENTIFIER}_{END}_{_ANY}_",
        f"_{IDENTIFIER}_{IDENTIFIER}_{_LETTERS}_", #overwrite the above any transition for letters
        
        # ambiguous
        f"_{START}_{AMBIGUOUS}_{_LOWERCASE}_",
        f"_{AMBIGUOUS}_{END}_{_ANY}_",
        f"_{AMBIGUOUS}_{IDENTIFIER}_{_UPPERCASE}_", #overwrite the above any transition for uppercase letters
        f"_{AMBIGUOUS}_{AMBIGUOUS}_{_LOWERCASE}_", #overwrite the above any transition for lowercase letters

        # comment
        f"_{DIV}_{COMMENT}_*_",
        f"_{COMMENT}_{COMMENT}_{_ANY}_",
        f"_{COMMENT}_{COMMENT_OUT}_*_", #overwrite the above any transition when leaving the comment
        f"_{COMMENT_OUT}_{COMMENT_OUT}_{_ANY}_",
        f"_{COMMENT_OUT}_{START}_/_", #overwrite the above any transition when leaving the comment
    ]

    _command = './main ' + ' '.join([f"\"{argument}\"" for argument in _ARGUMENTS])
    print(_command)
    os.system(_command)
    
    _command = 'echo "#ifndef STATES_DFA_H\n#define STATES_DFA_H\n typedef enum{\n'
    for _variable, _value in locals().items():
        if(_variable[0] == '_'):
            continue
        _command = f"{_command}\t{_variable}=(unsigned char)'{_value[0]}',\n"
    
    _command = _command + f'\n}}dfa_states;\n#endif" > states_{_ARGUMENTS[0]}'
    print(_command)
    os.system(_command)
        
if __name__ == "__main__":
    configureDFA()