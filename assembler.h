#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "dictionary.h"

typedef struct assembler{
    dictionary ram_mapping;
    dictionary rom_mapping;
}assembler;

assembler createAssembler(size_t hash_size);
void asssemble(assembler a, );
#endif