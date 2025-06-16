#include "assembler.h"

size_t mapping(char * name, size_t size){
    size_t i = 0;
    size_t index = 0;
    size_t step = 1;

    while(name[i] != '\0'){
        index += name[i] * step;
        step *= 26;
        i++;
    }

    return index % size;
}

assembler createAssembler(size_t hash_size){
    assembler a = {
        .ram_mapping = createDict(hash_size,mapping),
        .rom_mapping = createDict(hash_size,mapping),
    };

    return a;
}