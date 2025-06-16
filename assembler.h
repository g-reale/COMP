#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "intermediary.h"
#include "dictionary.h"
#include "stack.h"

typedef struct variable{
    size_t addresss;
    size_t size;
    size_t value;
}variable;


typedef struct assembler{
    intermediary * inter;
    size_t ram_address;
    size_t rom_address;
    stack ram_context;
    stack rom_context;
    uint64_t * ram_map;
    uint64_t * rom_map;
    size_t ram_size;
    size_t rom_size;

}assembler;

assembler createAssembler(intermediary i, size_t hash_size);
void asssemble(assembler a);
#endif