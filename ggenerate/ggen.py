#!/usr/bin/env python3

import pprint
import pickle

# 1. programa → declaração-lista
# 2. declaração-lista → declaração-lista declaração|declaração
# 3. declaração → var-declaração|fun-declaração
# 4. var-declaração → tipo-especificador IDENTIFIER SEMI|tipo-especificador IDENTIFIER OPEN_SQUARE NUM CLOSE_SQUARE SEMI
# 5. tipo-especificador → int|void
# 6. fun-declaração → tipo-especificador IDENTIFIER OPEN_ROUND params CLOSE_ROUNDcomposto-decl
# 7. params → param-lista|void
# 8. param-lista → param-lista,param|param
# 9. param → tipo-especificador IDENTIFIER|tipo-especificador IDENTIFIER OPEN_SQUARE CLOSE_SQUARE
# 10. composto-decl → { local-declarações statement-lista }
# 11. local-declarações → local-declarações var-declaração|vazio
# 12. statement-lista → statement-lista statement|vazio
# 13. statement → expressão-decl|composto-decl|seleção-decl |iteração-decl|retorno-decl
# 14. expressão-decl → expressão SEMI|SEMI
# 15. seleção-decl →if OPEN_ROUND expressão CLOSE_ROUND statement |if OPEN_ROUND expressão CLOSE_ROUND statement else statement
# 16. iteração-decl → while OPEN_ROUND expressão CLOSE_ROUND statement
# 17. retorno-decl → return SEMI|return expressão SEMI
# 18. expressão → var = expressão|simples-expressão
# 19. var → IDENTIFIER|IDENTIFIER OPEN_SQUARE expressão CLOSE_SQUARE
# 20. simples-expressão → soma-expressão relacional soma-expressão |soma-expressão
# 21. relacional → LESS_EQ |< | >| MORE_EQ | LOGICAL_EQ |!=
# 22. soma-expressão → soma-expressão soma termo|termo
# 23. soma → SUM| SUB
# 24. termo → termo mult fator|fator
# 25. mult → MUL|DIV
# 26. fator → OPEN_ROUND expressão CLOSE_ROUND|var|ativação|NUM
# 27. ativação → IDENTIFIER OPEN_ROUND args CLOSE_ROUND
# 28. args → arg-lista|vazio
# 29. arg-lista → arg-lista,expressão|expressão

GRAMMAR = {
    "PROGRAM":               {("DECLARATION", "REPETITION", "DECLARATION", "REPETITION")},
    "DECLARATION":           {("VOID","IDENTIFIER","VOID_FUN_DECL"), ("INT", "IDENTIFIER", "DECLARATION_DECISION")},
    "DECLARATION_DECISION":  {("VEC_DECL",), ("SCALAR_DECL",), ("INT_FUN_DECL",)},
    
    "VEC_DECL":              {("OPEN_SQUARE", "NUM", "CLOSE_SQUARE", "SEMI")},
    "SCALAR_DECL":           {("SEMI",)},
    "VOID_FUN_DECL":         {("OPEN_ROUND", "PARAMS", "VOID_COMPOSED_DECL")},
    "INT_FUN_DECL":          {("OPEN_ROUND", "PARAMS", "INT_COMPOSED_DECL")},

    "PARAMS":                {("VOID","CLOSE_ROUND"), ("PARAM","REPETITION","PARAM","REPETITION")},
    "PARAM":                 {("INT", "IDENTIFIER","PARAM_DECISION")},
    "PARAM_DECISION":        {("SCALAR_PARAM",), ("VEC_PARAM",)},
    "SCALAR_PARAM":          {("COMMA",), ("CLOSE_ROUND",)},
    "VEC_PARAM":             {("OPEN_SQUARE","CLOSE_SQUARE","SCALAR_PARAM")},
    
    "INT_COMPOSED_DECL":     {("OPEN_CURLY", "OPTIONAL", "LOCAL_DECL", "OPTIONAL", "OPTIONAL", "INT_STATEMENT_LIST", "OPTIONAL", "CLOSE_CURLY")},
    "VOID_COMPOSED_DECL":    {("OPEN_CURLY", "OPTIONAL", "LOCAL_DECL", "OPTIONAL", "OPTIONAL", "VOID_STATEMENT_LIST", "OPTIONAL", "CLOSE_CURLY")},
    "LOCAL_DECL":            {("VAR_DECL", "REPETITION", "VAR_DECL", "REPETITION")},
    "VAR_DECL":              {("INT", "IDENTIFIER", "VAR_DECISION")},
    "VAR_DECISION":          {("VEC_DECL",), ("SCALAR_DECL",)},
    
    "INT_STATEMENT_LIST":    {("INT_STATEMENT", "REPETITION", "INT_STATEMENT", "REPETITION")},
    "INT_STATEMENT":         {("EXPRESSION_DECL",), ("INT_COMPOSED_DECL",), ("INT_SELECTION_DECL",), ("INT_ITERATION_DECL",), ("INT_RETURN_DECL",)},
    "INT_SELECTION_DECL":    {("IF", "OPEN_ROUND", "CONDITION", "CLOSE_ROUND", "INT_IF_BODY", "OPTIONAL", "ELSE", "INT_ELSE_BODY", "OPTIONAL")},
    "INT_IF_BODY":           {("INT_STATEMENT",)},
    "INT_ELSE_BODY":         {("INT_STATEMENT",)},
    "INT_ITERATION_DECL":    {("WHILE", "OPEN_ROUND", "CONDITION", "CLOSE_ROUND", "INT_STATEMENT")},
    "INT_RETURN_DECL":       {("RETURN", "EXPRESSION", "SEMI")},
    
    "VOID_STATEMENT_LIST":   {("VOID_STATEMENT", "REPETITION", "VOID_STATEMENT", "REPETITION")},
    "VOID_STATEMENT":        {("EXPRESSION_DECL",), ("VOID_COMPOSED_DECL",), ("VOID_SELECTION_DECL",), ("VOID_ITERATION_DECL",), ("VOID_RETURN_DECL",)},
    "VOID_SELECTION_DECL":   {("IF", "OPEN_ROUND", "CONDITION", "CLOSE_ROUND", "VOID_IF_BODY", "OPTIONAL", "ELSE", "VOID_ELSE_BODY", "OPTIONAL")},
    "VOID_IF_BODY":          {("VOID_STATEMENT",)},
    "VOID_ELSE_BODY":        {("VOID_STATEMENT",)},
    "VOID_ITERATION_DECL":   {("WHILE", "OPEN_ROUND", "CONDITION", "CLOSE_ROUND", "VOID_STATEMENT")},
    "VOID_RETURN_DECL":      {("RETURN", "SEMI")},
    "EXPRESSION_DECL":       {("EXPRESSION", "SEMI"), ("SEMI",)},

    "CONDITION":             {("EXPRESSION",)},
    "EXPRESSION":            {("SIMPLE_EXP", "REPETITION", "EQUAL", "SIMPLE_EXP", "REPETITION")}, #GRAMMAR LIMITATION :(
    "VAR":                   {("IDENTIFIER", "OPTIONAL", "OPEN_SQUARE", "EXPRESSION", "CLOSE_SQUARE", "OPTIONAL")},
    "SIMPLE_EXP":            {("SUM_EXP", "OPTIONAL", "RELATIONAL", "SUM_EXP", "OPTIONAL")},
    "RELATIONAL":            {("LESS_EQ",), ("LOGICAL_EQ",), ("MORE_EQ",), ("LOGICAL_EQ",), ("NOT_EQUAL",), ("LESS",), ("MORE",)},
    "SUM_EXP":               {("TERM", "REPETITION", "PLUS_MINUS", "TERM", "REPETITION")},
    "PLUS_MINUS":            {("SUM",), ("SUB",)},
    "TERM":                  {("FACTOR", "REPETITION", "MUL_DIV", "FACTOR", "REPETITION")},
    "MUL_DIV":               {("MUL",), ("DIV",)},
    "FACTOR":                {("OPEN_ROUND", "EXPRESSION", "CLOSE_ROUND"), ("IDENTIFIER", "OPTIONAL", "ACTIVATION_DECISION", "OPTIONAL"), ("NUM",)},
    "ACTIVATION_DECISION":   {("FUNCTION_ACTIVATION",), ("VECTOR_ACTIVATION",)},
    "VECTOR_ACTIVATION":     {("OPEN_SQUARE", "EXPRESSION", "CLOSE_SQUARE")},
    "FUNCTION_ACTIVATION":   {("OPEN_ROUND", "ARGUMENT_DECISION"),},
    "ARGUMENT_DECISION":     {("CLOSE_ROUND",),("ARGUMENT","REPETITION","COMMA","ARGUMENT","REPETITION","CLOSE_ROUND")},
    "ARGUMENT":              {("EXPRESSION",)}
}


FIRST = {}
PRODUCTION_MAP = {}
PRODUCTION_TABLE = []
IS_NON_TERMINAL = [0 for _ in range(256)]
LONGEST_PRODUCTION = 0

def calculateFirst(symbol):
    global LONGEST_PRODUCTION

    if symbol == "REPETITION" or symbol == "OPTIONAL":
        return set()

    if symbol not in GRAMMAR:
        return {(symbol)}

    if symbol in FIRST:
        return FIRST[symbol]

    FIRST[symbol] = set()
    PRODUCTION_MAP[symbol] = []

    for production in GRAMMAR[symbol]:
        balanced = True
        production_first = set()
        
        for index,string in enumerate(production):
            
            if string == "OPTIONAL" or string == "REPETITION":
                production_first = production_first | calculateFirst(production[index + 1])         #rules that can be empty will generate a error (no bound check)
                balanced = not balanced

            if balanced:
               production_first = production_first | calculateFirst(string)
               break

        
        LONGEST_PRODUCTION = max(len(production),LONGEST_PRODUCTION)
        PRODUCTION_MAP[symbol].append((production_first,production))
        FIRST[symbol] = FIRST[symbol] | production_first
        
    return FIRST[symbol]

for non_terminal in GRAMMAR:
    calculateFirst(non_terminal)

# import state mapping
MAPPING = {} 
with open("../dgenerate/mapping.pkl","rb") as file:
    MAPPING = pickle.load(file)

# create a production table using the production map and the state mapping
PRODUCTION_TABLE = [[("FAILURE",) for _ in range(256)] for _ in range(256)]
for non_terminal, paths in PRODUCTION_MAP.items():
    IS_NON_TERMINAL[MAPPING[non_terminal]] = 1
    for path in paths:
        [terminals,production] = path
        for terminal in terminals:
            PRODUCTION_TABLE[MAPPING[non_terminal]][MAPPING[terminal]] = production
            # print(non_terminal,terminal,MAPPING[non_terminal],MAPPING[terminal],PRODUCTION_TABLE[MAPPING[non_terminal]][MAPPING[terminal]])

# create a fist table using the fist and state mapping
FIRST_TABLE = [[1 if i == j else 0 for i in range(256)] for j in range(256)]
for non_terminal, first in FIRST.items():
    for terminal in first:
        FIRST_TABLE[MAPPING[non_terminal]][MAPPING[terminal]] = 1


            
# export the production table and the first table to a C header 
with open("production_table.h","w") as file:
    program = f"""
#ifndef GRAMMAR_H
#define GRAMMAR_H
#include "../dgenerate/states_dfa.h" 
dfa_states PARSER[256][256][{LONGEST_PRODUCTION+1}] = {{"""

    for line in PRODUCTION_TABLE:
        program += "{"
        for column in line:
            program += "{" + ",".join(tuple(str(element) for element in column)) +  ", END},"
        program = program[:-1]
        program += "},\n"
    program = program[:-3]
    program += "}};\n"

    program += "unsigned char IS_NON_TERMINAL[256]={"
    program += ",".join(map(str, IS_NON_TERMINAL))
    program+= "};\n"


    program += "unsigned char IN_FIRST[256][256]={"
    for line in FIRST_TABLE:
        program += "{" + ",".join(map(str,line)) + "},\n"
    program = program[:-2]
    program += "};\n#endif"

    file.write(program)


print("firsts\n\n")
pprint.pp(FIRST)
print("production map\n\n")
pprint.pp(PRODUCTION_MAP)