#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "intermediary.h"
#include "dictionary.h"
#include "stack.h"

#define PC_ADDR 0
#define ARG_STACK_ADDR 1
#define CALL_STACK_ADDR 2
#define ONE_CT_ADDR 3
#define THREE_CT_ADDR 4
#define MISC_REGISTER_ADDR 5
#define RETURN_REGISTER_ADDR 6
#define DISP_ADDR 7
#define SWITCH_ADDR 8
#define VEC_DEF_ADDR_A 9
#define VEC_DEF_ADDR_B 10
#define CONSTANT_MEMORY_START 11

#define CALLSTACK_SIZE 128
#define ARGUMENT_STACK_SIZE 128
#define MEMORY_SIZE 1024


typedef enum{
    ASM_ADD     = 0,
    ASM_SUB     = 1,
    ASM_MUL     = 2,
    ASM_DIV     = 3,
    ASM_FJMP    = 4,
    ASM_LT      = 5,
    ASM_GT      = 6,
    ASM_LEQ     = 7,
    ASM_GEQ     = 8,
    ASM_EQ      = 9,
    ASM_NEQ     = 10,
    ASM_SET     = 11,
    ASM_SETDS   = 12,
    ASM_SETDD   = 13,
    ASM_SETDDI  = 14,
    ASM_SETI    = 15
}asm_t;

typedef struct {
    asm_t instruction;
    size_t destination;
    size_t source_A;
    size_t source_B;
}ins_t;

typedef struct assembler{
    dictionary varmap;
    size_t varat;
    size_t progat;
    stack context;
    intermediary * inter;
    ins_t * assembly;
    size_t * memory;
    int quiet;
    FILE * out;
}assembler;


assembler createAssembler(char * path, intermediary * i, int quiet);
void assemble(assembler * a);
void destroyAssembler(assembler * a);

#endif