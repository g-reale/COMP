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

void destructor(void * value){
    dictionary dict = *((dictionary*)value);
    destroyDict(dict);
}

assembler createAssembler(intermediary * i, size_t ram_size, size_t rom_size){
    assembler a = {
        .inter = i,
        .ram_context = createStack(destructor),
        .ram_address = 0,
        .ram_map = (uint64_t*)calloc(ram_size, sizeof(uint64_t)),
        .ram_size = ram_size
    };

    //create main context latter

    return a;
}

void deleteAssembler(assembler a){
    destroyIntermediary(a.inter);
    destroyStack(&a.ram_context);
}

void assemble(assembler * a){
    for(size_t i = 0; i < a->inter->code_size; i++){
        quadruple quad = a->inter->pseudo_asm[i];
        
        switch (quad.instruction){
            
            //allocate space for a new variable 
            case SCALAR_DECL:
            case VEC_DECL:{
                void * address = (void*)malloc(sizeof(size_t));
                *(size_t *)address = a->ram_address;
                a->ram_address += quad.instruction == VEC_DECL ? (size_t)strtoull(quad.source_A, NULL, 10) : 1;
                dictionary dict = *((dictionary*)top(&a->ram_context));
                insertDict(dict,quad.destination,address,free);
            }
            break;

            //push or pop context
            case INT_FUN_DECL:
            case VOID_FUN_DECL:{
                
                //pop context
                if(quad.destination == NULL){
                    pop(&a->ram_context);
                    break;
                }
                
                //push context
                dictionary * new_ctx = malloc(sizeof(dictionary));
                *new_ctx = createDict(512,mapping);
                push(&a->ram_context,new_ctx);
            }
            break;

            
        }
    }
}
