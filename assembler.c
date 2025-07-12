#include "assembler.h"

assembler createAssembler(intermediary * i){
    assembler a = {
        .varmap = createDict(),
        .progmap = createDict(),
        .context = 0,
        .inter = i,
        .varat = CONSTANT_MEMORY_START,
        .progat = 0,
    };
    return a;
}

void assemble(assembler * a){

    //first pass solve memory adresses
    //registering addresses and allocating space by quadruple type
    for(size_t i = 0; i < a->inter->code_size; i++){
        quadruple quad = a->inter->pseudo_asm[i];
        
        switch (quad.instruction){
            
            //changing the context
            case VOID_FUN_DECL:
            case INT_FUN_DECL:{
                // opening function
                if(quad.destination){
                    size_t leaf;
                    insertDict(&a->varmap,quad.destination,(entry_t){.type = SUBTREE},0,&leaf);
                    insertDict(&a->progmap,quad.destination,(entry_t){.type = ADDRESS, (data_t){.address = a->progat}},0,NULL);
                    a->context = leaf;
                    break;
                }

                //closing function
                a->context = 0;
                a->progat+=3;
            }break;
            
            //inserting a vector in the dict
            case VEC_DECL:{
                size_t offset = (size_t)strtoull(quad.source_A, NULL, 10);
                insertDict(&a->varmap,quad.destination,(entry_t){.type = ADDRESS, .entry = {.address = a->varat, .size = offset}},a->context,NULL);
                a->varat += offset;
            }break;

            //inserting vector parameters/scalars in the dict
            case VEC_PARAM:
            case SCALAR_PARAM:
                a->progat+=2;
            case SCALAR_DECL:{
                insertDict(&a->varmap,quad.destination,(entry_t){.type = ADDRESS, .entry = {.address = a->varat, .size = 1}},a->context,NULL);
                a->varat++;
            }break;

            case EQUAL:
            case MUL:
            case DIV:
            case LESS:
            case LESS_EQ:
            case MORE:
            case MORE_EQ:
            case SUM:
            case SUB:
            case LOGICAL_EQ:
            case RETURN:{
                char * names[3] = {quad.destination,quad.source_A,quad.source_B};
                for(size_t i = 0; i < 3; i++){
                    if(names[i] != NULL)
                        a->varat += insertDict(&a->varmap,names[i],(entry_t){.type = ADDRESS, .entry = {.address = a->varat, .size = 1}},a->context,NULL);
                }
                a->progat++;
            }break;

            case ARGUMENT:
                a->progat+=2;
                break;

            case FUNCTION_ACTIVATION:
                insertDict(&a->varmap,quad.destination,(entry_t){.type = ADDRESS, .entry = {.address = a->varat, .size = 1}},a->context,NULL);
                a->varat++;
                a->progat+=5;
                break;

            default:
                break;
        }
    }

    a->assembly = (ins_t*)calloc(sizeof(ins_t),a->progat);

    //second pass convert to instructions using resolved addresses
    size_t j = 0;
    for(size_t i = 0; i < a->inter->code_size; i++){
        quadruple quad = a->inter->pseudo_asm[i];
        asm_t opcode;

        switch (quad.instruction){


            //changing the context and maybe jumping back
            case VOID_FUN_DECL:
            case INT_FUN_DECL:{

                // opening function
                if(quad.destination){
                    size_t leaf;
                    querryDict(&a->varmap,quad.destination,NULL,0,&leaf);
                    a->context = leaf;
                    break;
                }

                //closing function
                //get the stack pointer
                a->assembly[j] = (ins_t){
                    .instruction = ASM_SETDS,
                    .destination = MISC_REGISTER_ADDR,
                    .source_A = CALL_STACK_ADDR,
                };
                j++;

                //decrease the stack pointer
                a->assembly[j] = (ins_t){
                    .instruction = ASM_SUB,
                    .destination = CALL_STACK_ADDR,
                    .source_A = CALL_STACK_ADDR,
                    .source_B = ONE_CT_ADDR
                };
                j++;

                //set the PC to the stack top value
                a->assembly[j] = (ins_t){
                    .instruction = ASM_SET,
                    .destination = PC_ADDR,
                    .source_A = MISC_REGISTER_ADDR,
                };
                j++;
                
                a->context = 0;
            }break;

            case EQUAL:
            case MUL:
            case DIV:
            case LESS:
            case LESS_EQ:
            case MORE:
            case MORE_EQ:
            case SUM:
            case SUB:
            case LOGICAL_EQ:{

                opcode = quad.instruction == EQUAL ? ASM_SET :
                         quad.instruction == MUL ? ASM_MUL :
                         quad.instruction == DIV ? ASM_DIV :
                         quad.instruction == LESS ? ASM_LT :
                         quad.instruction == LESS_EQ ? ASM_LEQ :
                         quad.instruction == MORE ? ASM_GT :
                         quad.instruction == MORE_EQ ? ASM_GEQ :
                         quad.instruction == SUM ? ASM_ADD :
                         quad.instruction == SUB ? ASM_SUB :
                         ASM_EQ;
                
                a->assembly[j] = (ins_t){
                    .instruction = opcode,
                    .destination = querryDict(&a->varmap,quad.destination,NULL,a->context,NULL).entry.address,
                    .source_A = querryDict(&a->varmap,quad.source_A,NULL,a->context,NULL).entry.address,
                    .source_B = querryDict(&a->varmap,quad.source_B,NULL,a->context,NULL).entry.address,
                };
                j++;
            } break;

            //copy a vaule to the return register
            case RETURN:{
                a->assembly[j] = (ins_t){
                    .instruction = ASM_SET,
                    .destination = RETURN_REGISTER_ADDR,
                    .source_A = querryDict(&a->varmap,quad.destination,NULL,a->context,NULL).entry.address,
                };
                j++;
            } break;

            //stack a argument
            case ARGUMENT:{

                //inscrement argument stack pointer
                a->assembly[j] = (ins_t){
                    .instruction = ASM_ADD,
                    .destination = ARG_STACK_ADDR,     
                    .source_A = ARG_STACK_ADDR,        
                    .source_B = ONE_CT_ADDR
                };
                j++;

                //stack scalar or vector (copy/reference)
                data_t variable = querryDict(&a->varmap,quad.destination,NULL,a->context,NULL).entry;
                opcode = 1 == variable.size ? ASM_SETDD : ASM_SETDDI;
                a->assembly[j] = (ins_t){
                    .instruction = opcode,
                    .destination = ARG_STACK_ADDR,
                    .source_A = variable.address
                };
                j++;
            }break;

            //pop parameters
            case SCALAR_PARAM:
            case VEC_PARAM:{
                a->assembly[j] = (ins_t){
                    .instruction = ASM_SETDS,
                    .destination = querryDict(&a->varmap,quad.destination,NULL,a->context,NULL).entry.address,
                    .source_A = ARG_STACK_ADDR,
                };
                j++;

                a->assembly[j] = (ins_t){
                    .instruction = ASM_SUB,
                    .destination = ARG_STACK_ADDR,
                    .source_A = ARG_STACK_ADDR,
                    .source_B = ONE_CT_ADDR
                };
                j++;
            }break;

            //jump to function start
            case FUNCTION_ACTIVATION:{
                
                //increment the call stack top
                a->assembly[j] = (ins_t){
                    .instruction = ASM_ADD,
                    .destination = CALL_STACK_ADDR,
                    .source_A = CALL_STACK_ADDR,
                    .source_B = ONE_CT_ADDR
                };
                j++;

                //calculte the PC after the function call
                a->assembly[j] = (ins_t){
                    .instruction = ASM_ADD,
                    .destination = MISC_REGISTER_ADDR,
                    .source_A = PC_ADDR,
                    .source_B = THREE_CT_ADDR
                };
                j++;

                //set the call stack to point back to the calculated pc position
                a->assembly[j] = (ins_t){
                    .instruction = ASM_SETDD,
                    .destination = CALL_STACK_ADDR,
                    .source_A = MISC_REGISTER_ADDR,
                };
                j++;

                //perform the jump into the function
                a->assembly[j] = (ins_t){
                    .instruction = ASM_SET,
                    .destination = PC_ADDR,
                    .source_A = querryDict(&a->progmap,quad.source_A,NULL,0,NULL).entry.address,
                };
                j++;
                
                a->assembly[j] = (ins_t){
                    .instruction = ASM_SET,
                    .destination = querryDict(&a->varmap,quad.destination,NULL,0,NULL).entry.address,
                    .source_A = MISC_REGISTER_ADDR,
                };
                j++;

            }break;
        }
    }
}

void destroyAssembler(assembler * a){
    
    traverseDict(&a->varmap,0);
    printf("\n");
    traverseDict(&a->progmap,0);

    printf("\n");
    char opcode_names[16][32] = {
        "ASM_ADD",
        "ASM_SUB",
        "ASM_MUL",
        "ASM_DIV",
        "ASM_LT",
        "ASM_GT",
        "ASM_LEQ",
        "ASM_GEQ",
        "ASM_EQ",
        "ASM_NEQ",
        "ASM_JMP",
        "Nan",
        "ASM_SET",
        "ASM_SETDS",
        "ASM_SETDD",
        "ASM_SETDDI",
    };

    char reserved_addresses_names[16][64] = {
        "PC",
        "A_STACK",
        "C_STACK",
        "ONE",
        "THREE",
        "MISC_R",
        "RET_R",
    };
    
    for(size_t i = 0; i < a->progat; i++){
        printf("%s(%ld)\t",opcode_names[a->assembly[i].instruction],a->assembly[i].instruction);
        size_t addresses[3] = {a->assembly[i].destination,a->assembly[i].source_A,a->assembly[i].source_B};
        for(size_t i = 0; i < 3; i++){

            if(i == 2 && ASM_SET <= a->assembly[i].instruction)
                continue;

            if(addresses[i] < CONSTANT_MEMORY_START)
                printf("%s\t",reserved_addresses_names[addresses[i]]);
            else
                printf("%ld\t",addresses[i]);
        }
        printf("\n");
    }
    printf("\n");

    destroyIntermediary(a->inter);
    destroyDict(&a->progmap);
    destroyDict(&a->varmap);
}


// // size_t mapping(char * name, size_t size){
// //     size_t i = 0;
// //     size_t index = 0;
// //     size_t step = 1;

// //     while(name[i] != '\0'){
// //         index += name[i] * step;
// //         step *= 26;
// //         i++;
// //     }

// //     return index % size;
// // }

// // void destructor(void * value){
// //     dictionary dict = *((dictionary*)value);
// //     destroyDict(dict);
// // }

// // assembler createAssembler(intermediary * i, size_t ram_size, size_t rom_size){
// //     assembler a = {
// //         .inter = i,
// //         .ram_context = createStack(destructor),
// //         .ram_address = 0,
// //         .ram_map = (uint64_t*)calloc(ram_size, sizeof(uint64_t)),
// //         .ram_size = ram_size
// //     };

// //     //create main context latter

// //     return a;
// // }

// // void deleteAssembler(assembler a){
// //     destroyIntermediary(a.inter);
// //     destroyStack(&a.ram_context);
// // }

// // void assemble(assembler * a){
// //     for(size_t i = 0; i < a->inter->code_size; i++){
// //         quadruple quad = a->inter->pseudo_asm[i];
        
// //         switch (quad.instruction){
            
// //             //allocate space for a new variable 
// //             case SCALAR_DECL:
// //             case VEC_DECL:{
// //                 void * address = (void*)malloc(sizeof(size_t));
// //                 *(size_t *)address = a->ram_address;
// //                 a->ram_address += quad.instruction == VEC_DECL ? (size_t)strtoull(quad.source_A, NULL, 10) : 1;
// //                 dictionary dict = *((dictionary*)top(&a->ram_context));
// //                 insertDict(dict,quad.destination,address,free);
// //             }
// //             break;

// //             //push or pop context
// //             case INT_FUN_DECL:
// //             case VOID_FUN_DECL:{
                
// //                 //pop context
// //                 if(quad.destination == NULL){
// //                     pop(&a->ram_context);
// //                     break;
// //                 }
                
// //                 //push context
// //                 dictionary * new_ctx = malloc(sizeof(dictionary));
// //                 *new_ctx = createDict(512,mapping);
// //                 push(&a->ram_context,new_ctx);
// //             }
// //             break;

            
// //         }
// //     }
// // }
