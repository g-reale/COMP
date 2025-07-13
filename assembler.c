#include "assembler.h"

assembler createAssembler(intermediary * i){
    assembler a = {
        .varmap = createDict(),
        .context = 0,
        .inter = i,
        .varat = CONSTANT_MEMORY_START,
        .progat = 0,
    };

    // DMA variables
    insertDict(&a.varmap,"DISPADDR",(entry_t){.type = CONSTANT, .data = (data_t){.address = DISP_ADDR, .size = 1, .value = 0}},0,NULL);
    insertDict(&a.varmap,"SWICHADDR",(entry_t){.type = CONSTANT, .data = (data_t){.address = SWITCH_ADDR, .size = 1, .value = 0}},0,NULL);
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
                    insertDict(&a->varmap,quad.destination,(entry_t){.type = FUNCTION, (data_t){.address = a->varat, .size = 1, .value = a->progat}},0,&leaf);
                    a->varat++;
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
                insertDict(&a->varmap,quad.destination,(entry_t){.type = VARIABLE, .data = {.address = a->varat, .size = offset}},a->context,NULL);
                a->varat += offset;
            }break;

            //inserting vector parameters/scalars in the dict
            case VEC_PARAM:
            case SCALAR_PARAM:
                a->progat+=2;
            [[fallthrough]];
            case SCALAR_DECL:{

                insertDict(&a->varmap,quad.destination,
                (entry_t){.type = VARIABLE, .data = {
                            .address = a->varat, 
                            .size = 1, 
                            .value = isdigit(quad.destination[0]) ? (size_t)strtoull(quad.destination, NULL, 10) : 0
                            }
                        },a->context,NULL);
                a->varat++;
            }break;

            case RETURN:
                a->progat += 3;
            [[fallthrough]];
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
                char * names[3] = {quad.destination,quad.source_A,quad.source_B};
                for(size_t i = 0; i < 3; i++){
                    if(names[i] != NULL && !isalpha(names[i][0])){
                        int digit = isdigit(names[i][0]);
                        a->varat += insertDict(&a->varmap,names[i],
                                    (entry_t){
                                        .type = digit ? CONSTANT : VARIABLE, 
                                        .data = {
                                            .address = a->varat, 
                                            .size = 1, 
                                            .value = digit ? (size_t)strtoull(names[i], NULL, 10) : 0
                                        }
                                    },
                                    digit ? 0 : a->context, NULL);
                    }
                }
                a->progat++;
            }break;

            case ARGUMENT:{
                int digit = isdigit(quad.destination[0]);
                if(digit){
                    a->varat += insertDict(&a->varmap,quad.destination,
                                (entry_t){
                                    .type = CONSTANT, 
                                    .data = {
                                        .address = a->varat, 
                                        .size = 1, 
                                        .value = (size_t)strtoull(quad.destination, NULL, 10)
                                    }
                                },0,NULL);
                }
                a->progat+=2;
            }break;

            case FUNCTION_ACTIVATION:
                insertDict(&a->varmap,quad.destination,
                (entry_t){.type = VARIABLE, .data = {
                            .address = a->varat, 
                            .size = 1, 
                            .value = isdigit(quad.destination[0]) ? (size_t)strtoull(quad.destination, NULL, 10) : 0
                        }
                },a->context,NULL);
                a->varat++;
                a->progat+=5;
                break;
            
            //create  a label in at the current line
            case LABEL:
                insertDict(&a->varmap,quad.destination,
                    (entry_t){.type = PPOINTER, .data = {
                                .address = a->varat, 
                                .size = 1, 
                                .value = a->progat  //store the current line
                            }
                    },a->context,NULL);
                a->varat++;
            break;
            
            //allocate space for a jump
            case IF:
            case GOTO:
                a->progat++;
            break;

            //allocate space for a vector activation
            case VECTOR_ACTIVATION:
                a->progat += 2;
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
                    entry_t function = querryDict(&a->varmap,quad.destination,NULL,0,&leaf);
                    a->varmap.arena[leaf].entry.data.value = function.data.value + a->varat + CONSTANT_MEMORY_START;    //offsetting program instructions
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
                    .destination = querryDict(&a->varmap,quad.destination,NULL,a->context,NULL).data.address,
                    .source_A = querryDict(&a->varmap,quad.source_A,NULL,a->context,NULL).data.address,
                    .source_B = querryDict(&a->varmap,quad.source_B,NULL,a->context,NULL).data.address,
                };
                j++;
            } break;

            //copy a value to the return register and terminate the function
            case RETURN:{
                
                //copy to return reg
                a->assembly[j] = (ins_t){
                    .instruction = ASM_SET,
                    .destination = RETURN_REGISTER_ADDR,
                    .source_A = querryDict(&a->varmap,quad.destination,NULL,a->context,NULL).data.address,
                };
                j++;

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
                data_t variable = querryDict(&a->varmap,quad.destination,NULL,a->context,NULL).data;
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
                    .destination = querryDict(&a->varmap,quad.destination,NULL,a->context,NULL).data.address,
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
                    .source_A = querryDict(&a->varmap,quad.source_A,NULL,0,NULL).data.address,
                };
                j++;
                
                //recuperate the value in the return register
                a->assembly[j] = (ins_t){
                    .instruction = ASM_SET,
                    .destination = querryDict(&a->varmap,quad.destination,NULL,a->context,NULL).data.address,
                    .source_A = RETURN_REGISTER_ADDR,
                };
                j++;

            }break;

            //fjmp to source B 
            case IF:{
                a->assembly[j] = (ins_t){
                    .instruction = ASM_FJMP,
                    .destination = MISC_REGISTER_ADDR,
                    .source_A = querryDict(&a->varmap,quad.destination,NULL,a->context,NULL).data.address,
                    .source_B = querryDict(&a->varmap,quad.source_B,NULL,a->context,NULL).data.address,
                };
                j++;
            }break;

            //inconditional PC set
            case GOTO:{
                size_t leaf;
                entry_t label = querryDict(&a->varmap,quad.destination,NULL,a->context,&leaf);
                a->varmap.arena[leaf].entry.data.value = label.data.value + a->varat + CONSTANT_MEMORY_START;   //offsetting program instructions
                a->assembly[j] = (ins_t){
                    .instruction = ASM_SET,
                    .destination = PC_ADDR,
                    .source_A = label.data.address,
                };
                j++;
            }break;

            
            case VECTOR_ACTIVATION:{
                a->assembly[j] = (ins_t){
                    .instruction = ASM_SETI,
                    .destination = MISC_REGISTER_ADDR,
                    .source_A = querryDict(&a->varmap,quad.source_A,NULL,a->context,NULL).data.address,
                };
                j++;

                a->assembly[j] = (ins_t){
                    .instruction = ASM_ADD,
                    .destination = querryDict(&a->varmap,quad.destination,NULL,a->context,NULL).data.address,
                    .source_A = MISC_REGISTER_ADDR,
                    .source_B = querryDict(&a->varmap,quad.source_B,NULL,a->context,NULL).data.address,
                };
                j++;
            }break;

            default:
                break;
        }
    }

    //layout constant memory
    a->memory = (size_t *)calloc(sizeof(size_t),a->varat);
    for(size_t i = 0; i < a->varmap.height; i++){
        if(a->varmap.arena[i].filled){
            entry_t variable = a->varmap.arena[i].entry;
            a->memory[variable.data.address] = variable.data.value;
        }
    }
    a->memory[PC_ADDR] = querryDict(&a->varmap,"main",NULL,0,NULL).data.value;
}

void destroyAssembler(assembler * a){
    
    traverseDict(&a->varmap,0);
    printf("\nPMEM @ %ld\n",a->varat);
    
    for(size_t i = 0; i < a->varat; i++)
        printf("%ld: %ld\n",i,a->memory[i]);

    char opcode_names[16][32] = {
        "ASM_ADD",
        "ASM_SUB",
        "ASM_MUL",
        "ASM_DIV",
        "ASM_FJMP",
        "ASM_LT",
        "ASM_GT",
        "ASM_LEQ",
        "ASM_GEQ",
        "ASM_EQ",
        "ASM_NEQ",
        "ASM_SET",
        "ASM_SETDS",
        "ASM_SETDD",
        "ASM_SETDDI",
        "ASM_SETI"
    };

    char reserved_addresses_names[16][64] = {
        "PC",
        "A_STACK",
        "C_STACK",
        "ONE",
        "THREE",
        "MISC_R",
        "RET_R",
        "DISP_ADDR",
        "SWICH_ADDR"
    };
    
    for(size_t i = 0; i < a->progat; i++){
        printf("%ld: %s(%ld)\t",i + a->varat, opcode_names[a->assembly[i].instruction],a->assembly[i].instruction);
        size_t addresses[3] = {a->assembly[i].destination,a->assembly[i].source_A,a->assembly[i].source_B};
        for(size_t j = 0; j < 3; j++){

            if(j == 2 && ASM_NEQ < a->assembly[i].instruction)
                continue;

            if(addresses[j] < CONSTANT_MEMORY_START)
                printf("%s(%ld)\t",reserved_addresses_names[addresses[j]],addresses[j]);
            else
                printf("%ld\t",addresses[j]);
        }
        printf("\n");
    }
    printf("\n");

    destroyIntermediary(a->inter);
    destroyDict(&a->varmap);
    free(a->assembly);
    free(a->memory);
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
