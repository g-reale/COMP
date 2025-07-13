#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "intermediary.h"
#include "dictionary.h"

#define PC_ADDR 0
#define ARG_STACK_ADDR 1
#define CALL_STACK_ADDR 2
#define ONE_CT_ADDR 3
#define THREE_CT_ADDR 4
#define MISC_REGISTER_ADDR 5
#define RETURN_REGISTER_ADDR 6
#define DISP_ADDR 7
#define SWITCH_ADDR 8
#define CONSTANT_MEMORY_START 9


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
    size_t context;
    intermediary * inter;
    ins_t * assembly;
    size_t * memory;
}assembler;


assembler createAssembler(intermediary * i);
void assemble(assembler * a);
void destroyAssembler(assembler * a);


// typedef struct variable{
//     size_t addresss;
//     size_t size;
//     size_t value;
// }variable;


// typedef struct assembler{
//     intermediary * inter;
//     size_t ram_address;
//     size_t rom_address;
//     stack ram_context;
//     stack rom_context;
//     uint64_t * ram_map;
//     uint64_t * rom_map;
//     size_t ram_size;
//     size_t rom_size;

// }assembler;

// assembler createAssembler(intermediary i, size_t hash_size);
// void asssemble(assembler a);
#endif