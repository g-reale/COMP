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

import subprocess
import pickle

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
    _WHITE_SPACE = "\n\t "
    _PROHIBITED = "!'$%&.:?@^_`|\\~"
    _PROHIBITED += '"'

    # Start and end
    START = chr(32)
    END = chr(33)
    FAILURE = chr(34)

    # Single character tokens
    SUM = chr(35)
    SUB = chr(36)
    MUL = chr(37)
    DIV = chr(38)
    LESS = chr(39)
    MORE = chr(40)
    SEMI = chr(41)
    COMMA = chr(42)
    EQUAL = chr(43)
    OPEN_ROUND = chr(44)
    CLOSE_ROUND = chr(45)
    OPEN_SQUARE = chr(46)
    CLOSE_SQUARE = chr(47)
    OPEN_CURLY = chr(48)
    CLOSE_CURLY = chr(49)

    _SINGLE_CHARACTER_TOKENS = SUM + SUB + MUL + DIV + LESS + MORE + SEMI + COMMA + EQUAL + OPEN_ROUND + CLOSE_ROUND + OPEN_SQUARE + CLOSE_SQUARE + OPEN_CURLY + CLOSE_CURLY

    # Double character tokens
    LESS_EQ = chr(50)
    MORE_EQ = chr(51)
    LOGICAL_EQ = chr(52)
    NOT = chr(53)
    NOT_EQUAL = chr(54)

    _DOUBLE_CHARACTER_TOKENS = LESS_EQ + MORE_EQ + LOGICAL_EQ + NOT_EQUAL

    # Numbers
    NUM = chr(55)

    # Ambiguous/identifiers/keywords
    IDENTIFIER = chr(56)
    AMBIGUOUS = chr(57)

    # Comments
    COMMENT = chr(58)
    COMMENT_OUT = chr(59)

    # Keywords
    IF = chr(60)
    INT = chr(61)
    VOID = chr(62)
    ELSE = chr(63)
    WHILE = chr(64)
    RETURN = chr(65)

    # Grammar non-terminals
    PROGRAM = chr(66)
    DECLARATION = chr(67)
    DECLARATION_DECISION = chr(68)
    VEC_DECL = chr(69)
    SCALAR_DECL = chr(70)
    VOID_FUN_DECL = chr(71)
    INT_FUN_DECL = chr(72)
    
    PARAMS = chr(73)
    PARAM = chr(74)
    PARAM_DECISION = chr(75)
    SCALAR_PARAM = chr(76)
    VEC_PARAM = chr(77)
    
    INT_COMPOSED_DECL = chr(78)
    VOID_COMPOSED_DECL = chr(79)
    LOCAL_DECL = chr(80)
    VAR_DECL = chr(81)
    VAR_DECISION = chr(82)
    INT_STATEMENT_LIST = chr(83)
    INT_STATEMENT = chr(84)
    INT_SELECTION_DECL = chr(85)
    INT_ITERATION_DECL = chr(86)
    INT_RETURN_DECL = chr(87)
    VOID_STATEMENT_LIST = chr(88)
    VOID_STATEMENT = chr(89)
    VOID_SELECTION_DECL = chr(90)
    VOID_ITERATION_DECL = chr(91)
    VOID_RETURN_DECL = chr(92)
    EXPRESSION_DECL = chr(93)
    EXPRESSION = chr(94)
    VAR = chr(95)
    SIMPLE_EXP = chr(96)
    RELATIONAL = chr(97)
    SUM_EXP = chr(98)
    PLUS_MINUS = chr(99)
    TERM = chr(100)
    MUL_DIV = chr(101)
    FACTOR = chr(102)
    ACTIVATION_DECISION = chr(103)
    VECTOR_ACTIVATION = chr(104)
    FUNCTION_ACTIVATION = chr(105)
    ARGUMENT_DECISION = chr(106)
    ARGUMENT = chr(107)
    
    VOID_IF_BODY = chr(108)
    VOID_ELSE_BODY = chr(109)
    INT_IF_BODY = chr(110)
    INT_ELSE_BODY = chr(111)
    CONDITION = chr(112)

    GOTO = chr(113)
    LABEL = chr(114)

    OPTIONAL = chr(115)
    REPETITION = chr(116)
 
    _ARGUMENTS = [
        #C program executable
        "./main",

        #output file name
        "dfa.h",
        
        # handle any non overwriten characters
        f"_{START}_{FAILURE}_{_ANY}_",
        f"_{START}_{START}_{_WHITE_SPACE}_",
        # f"_{START}_{FAILURE}_{_PROHIBITED}_",

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

    subprocess.run(_ARGUMENTS,check=True)
    # print(_ARGUMENTS)
    _state_amount = 0
    _name_len = 0
    _mapping = {}
    _program = "#ifndef STATES_DFA_H\n#define STATES_DFA_H\n typedef enum{\n"
    
    for _variable, _value in locals().items():
        if(_variable[0] == '_'):
            continue
        _program += f"\t{_variable}=(unsigned char){ord(_value)},\n"
        _mapping[_variable] = ord(_value);
        _name_len = max(_name_len,len(_variable))
        _state_amount += 1

    _program += "\n}dfa_states;\n"
    
    _program += f"static const char STATE_NAMES[{_state_amount+1}][{_name_len+1}] = {{"
    for _variable, _value in locals().items():
        if(_variable[0] == '_'):
            continue
        _program += f'"{_variable}", '
    _program = _program[:-2]
    _program += "};\n#endif"

    print(_program)

    # save the state mapping for other scripts
    with open("mapping.pkl","wb") as file:
        pickle.dump(_mapping,file)

    with open(f"states_{_ARGUMENTS[1]}","w") as file:
        file.write(_program)

        
if __name__ == "__main__":
    configureDFA()