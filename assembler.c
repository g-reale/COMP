#include "intermediary.h"
#include "assembler.h"
#include <ctype.h>

assembler createAssembler(char * path, intermediary * i, int quiet){
    
    //call intermediary before anything
    generate(i,i->root);
    
    //open output file
    FILE * fid = fopen(path,"w");
    if(fid == NULL){
        fprintf(stderr,"ERRO: arquivo %s não encontrado\n",path);
        destroyIntermediary(i);
        exit(1);
    }

    assembler a = {
        .varmap = createDict(),
        .context = createStack(),
        .inter = i,
        .varat = CONSTANT_MEMORY_START,
        .progat = 0,
        .quiet = quiet,
        .out = fid
    };
    push(&a.context,(entry_t){.type = CONTEXT, .data = (data_t){.value = 0}});
    
    // Hardware registers
    insertDict(&a.varmap,"DISPADDR",(entry_t){.type = CONSTANT, .data = (data_t){.address = DISP_ADDR, .size = 1, .value = 0}},0,NULL);
    insertDict(&a.varmap,"SWICHADDR",(entry_t){.type = CONSTANT, .data = (data_t){.address = SWITCH_ADDR, .size = 1, .value = 0}},0,NULL);
    return a;
}

entry_t traverseContexts(assembler * a, char * name, size_t * leaf){
    int bottom = 0;
    int found = 0;

    entry_t match;
    reset(&a->context);
    while(!bottom && !found){
        entry_t context = next(&a->context,&bottom);
        match = queryDict(&a->varmap,name,&found,context.data.value,leaf);
    }
    return match;
}

void writeBinary(assembler * a){
    for(size_t i = 0; i < a->varat; i++)
        fprintf(a->out,"ram[%lld] = 34'd%lld;\n",i,a->memory[i]);
    
    for(size_t i = 0; i < a->progat; i++){
        ins_t instruction = a->assembly[i];
        size_t value = ((size_t)(instruction.instruction) << 30U) + (instruction.destination << 20U) + (instruction.source_A << 10U) + instruction.source_B;
        fprintf(a->out,"ram[%lld] = 34'd%lld;\n",i + a->varat,value);
    }

    // for(size_t i = a->progat + a->varat; i < MEMORY_SIZE; i++)
    //     fprintf(a->out,"0\n");
}

// void writeBinary(assembler * a){
//     for(size_t i = 0; i < a->varat; i++)
//         fprintf(a->out,"%zx\n",a->memory[i]);
    
//     for(size_t i = 0; i < a->progat; i++){
//         ins_t instruction = a->assembly[i];
//         size_t value = ((size_t)(instruction.instruction) << 30U) + (instruction.destination << 20U) + (instruction.source_A << 10U) + instruction.source_B;
//         fprintf(a->out,"%zx\n",value);
//     }

// }

void assemble(assembler * a){

    //first pass solve memory addresses
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
                    push(&a->context,(entry_t){.type = CONTEXT, (data_t){.value = leaf}});
                    a->varat++;
                    break;
                }

                //closing function
                pop(&a->context,NULL);
                a->progat+=3;
            }break;

            //changing the context
            case VOID_COMPOSED_DECL:
            case INT_COMPOSED_DECL:{
                
                //opening context
                if(quad.destination){
                    size_t leaf;
                    insertDict(&a->varmap,quad.destination,(entry_t){.type = CONTEXT},a->context.top.data.value,&leaf);
                    push(&a->context,(entry_t){.type = CONTEXT, .data = (data_t){.value = leaf}});
                    break;
                }

                //closing context
                pop(&a->context,NULL);
            }break;
            
            //inserting a vector in the dict
            case VEC_DECL:{
                size_t offset = (size_t)strtoull(quad.source_A, NULL, 10);
                insertDict(&a->varmap,quad.destination,
                            (entry_t){
                                .type = VECTOR, 
                                .data = {
                                        .address = a->varat,
                                        .size = offset + 1, 
                                        .value = a->varat + 1 //point to vector start
                                    }
                                },
                            a->context.top.data.value,NULL);
                a->varat += offset + 1;
            }break;

            //inserting vector parameters/scalars in the dict
            case VEC_PARAM:
            case SCALAR_PARAM:
                a->progat+=2;
            [[fallthrough]];
            case SCALAR_DECL:{
                insertDict(&a->varmap,quad.destination,
                (entry_t){
                    .type = quad.instruction == VEC_PARAM ? VECTOR : VARIABLE, 
                    .data = {
                        .address = a->varat, 
                        .size = 1, 
                        .value = isdigit(quad.destination[0]) ? (size_t)strtoull(quad.destination, NULL, 10) : 0
                        }
                    },a->context.top.data.value,NULL);
                a->varat++;
            }break;

            //returning form a function
            case RETURN:{
                if(quad.destination != NULL && !isalpha(quad.destination[0])){
                    int digit = isdigit(quad.destination[0]);
                    a->varat += insertDict(&a->varmap,quad.destination,
                                (entry_t){
                                    .type = digit ? CONSTANT : VARIABLE, 
                                    .data = {
                                        .address = a->varat, 
                                        .size = 1, 
                                        .value = digit ? (size_t)strtoull(quad.destination, NULL, 10) : 0
                                    }
                                },
                                digit ? 0 : a->context.top.data.value, NULL);
                    }
                a->progat += 4;
            }break;

            //dirty work instructions
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
                    
                    //names can be empty for some of the opcodes
                    if(names[i] == NULL)
                        continue;
                    
                    //insert constants and implicit variables 
                    if(!isalpha(names[i][0])){
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
                            digit ? 0 : a->context.top.data.value, NULL);
                    }
                    
                    //allocate a extra instruction for each vector detected
                    a->progat += traverseContexts(a,names[i],NULL).type == VECTOR;
                }

                //always allocate at least one space for the main operation
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

            //allocate space for a function call and insert it's return variable in the dict
            case FUNCTION_ACTIVATION:
                insertDict(&a->varmap,quad.destination,
                (entry_t){.type = VARIABLE, .data = {
                            .address = a->varat, 
                            .size = 1, 
                            .value = 0
                        }
                },a->context.top.data.value,NULL);
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
                    },a->context.top.data.value,NULL);
                a->varat++;
            break;
            
            //allocate space for a jump
            case IF:
            case GOTO:
                a->progat++;
            break;

            //allocate space for a vector activation
            case VECTOR_ACTIVATION:{
                
                //insert the destination variable as a vector (access via reference)
                if(quad.destination[0] == '_')
                a->varat += insertDict(&a->varmap,quad.destination,
                    (entry_t){.type = VECTOR, 
                                .data = {
                                    .address = a->varat,
                                    .size = 1,
                                    .value = 0
                                }
                        },a->context.top.data.value,NULL);

                //insert the index variable, if constant insert in global memory
                if(!isalpha(quad.source_B[0])){
                    int digit = isdigit(quad.source_B[0]);
                    a->varat += insertDict(&a->varmap, quad.source_B,
                        (entry_t){.type = digit ? CONSTANT : VARIABLE,
                            .data = {
                                .address = a->varat,
                                .size = 1,
                                .value = digit ? (size_t)strtoull(quad.source_B, NULL, 10) : 0
                            }
                        },digit ? 0 : a->context.top.data.value, NULL);
                }

                //allocate space for a vector access
                a->progat ++;
            }break;

            default:
                break;
        }
    }

    //verify memory constrains
    size_t occupancy = a->progat + a->varat + CALLSTACK_SIZE + ARGUMENT_STACK_SIZE;
    if(MEMORY_SIZE < occupancy){
        fprintf(stderr,"assembler: program too big! (%lld/%d)\n",occupancy,MEMORY_SIZE);
        exit(1);
    }

    //allocate space for the assembly now that we know it's size
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
                    entry_t function = traverseContexts(a,quad.destination,&leaf);
                    a->varmap.arena[leaf].entry.data.value = function.data.value + a->varat;    //offsetting program instructions
                    push(&a->context,(entry_t){.type = CONTEXT, (data_t){.value = leaf}});
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
                
                //closing the context
                pop(&a->context,NULL);
            }break;

            //changing the context
            case VOID_COMPOSED_DECL:
            case INT_COMPOSED_DECL:{
                
                //opening context
                if(quad.destination){
                    size_t leaf;
                    traverseContexts(a,quad.destination,&leaf);
                    push(&a->context,(entry_t){.type = CONTEXT, .data = (data_t){.value = leaf}});
                    break;
                }

                //closing context
                pop(&a->context,NULL);
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

                entry_t sources[2] = {traverseContexts(a,quad.source_A,NULL),traverseContexts(a,quad.source_B,NULL)};
                size_t targets[2] = {sources[0].data.address,sources[1].data.address};
                
                //deference each source vector
                for(size_t k = 0; k < 2; k++){
                    if(sources[k].type == VECTOR){
                        a->assembly[j] = (ins_t){
                            .instruction = ASM_SETDS,
                            .destination = VEC_DEF_ADDR_A + k,
                            .source_A = targets[k],
                        };
                        targets[k] = VEC_DEF_ADDR_A + k;
                        j++;
                    }
                }

                entry_t destination = traverseContexts(a,quad.destination,NULL);
                
                //calculate the operation, store directly on destination if scalar, store on misc register if vector
                a->assembly[j] = (ins_t){
                    .instruction = opcode,
                    .destination = destination.type != VECTOR ? destination.data.address : MISC_REGISTER_ADDR,
                    .source_A = targets[0],
                    .source_B = targets[1],
                };
                j++;

                //destination is vector copy the result to the pointed position
                if(destination.type == VECTOR){
                    a->assembly[j] = (ins_t){
                        .instruction = ASM_SETDD,
                        .destination = destination.data.address,
                        .source_A = MISC_REGISTER_ADDR,
                    };
                    j++;
                }

            } break;

            //copy a value to the return register and terminate the function
            case RETURN:{
                
                //copy to return reg
                a->assembly[j] = (ins_t){
                    .instruction = ASM_SET,
                    .destination = RETURN_REGISTER_ADDR,
                    .source_A = traverseContexts(a,quad.destination,NULL).data.address,
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
                entry_t variable = traverseContexts(a,quad.destination,NULL);
                //opcode = variable.type == VECTOR ? ASM_SETDDI : ASM_SETDD;
                a->assembly[j] = (ins_t){
                    .instruction = ASM_SETDD,
                    .destination = ARG_STACK_ADDR,
                    .source_A = variable.data.address
                };
                j++;
            }break;

            //pop parameters
            case SCALAR_PARAM:
            case VEC_PARAM:{
                a->assembly[j] = (ins_t){
                    .instruction = ASM_SETDS,
                    .destination = traverseContexts(a,quad.destination,NULL).data.address,
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
                    .source_A = traverseContexts(a,quad.source_A,NULL).data.address,
                };
                j++;
                
                //recuperate the value in the return register
                a->assembly[j] = (ins_t){
                    .instruction = ASM_SET,
                    .destination = traverseContexts(a,quad.destination,NULL).data.address,
                    .source_A = RETURN_REGISTER_ADDR,
                };
                j++;

            }break;

            //fjmp to source B 
            case IF:{
                a->assembly[j] = (ins_t){
                    .instruction = ASM_FJMP,
                    .destination = MISC_REGISTER_ADDR,
                    .source_A = traverseContexts(a,quad.destination,NULL).data.address,
                    .source_B = traverseContexts(a,quad.source_B,NULL).data.address,
                };
                j++;
            }break;

            //inconditional PC set
            case GOTO:{
                a->assembly[j] = (ins_t){
                    .instruction = ASM_SET,
                    .destination = PC_ADDR,
                    .source_A = traverseContexts(a,quad.destination,NULL).data.address,
                };
                j++;
            }break;

            //offset the label reference to after the constant memory
            case LABEL:{
                size_t leaf;
                entry_t label = traverseContexts(a,quad.destination,&leaf);
                a->varmap.arena[leaf].entry.data.value = label.data.value + a->varat;   //offsetting program instructions
            }break;

            //access a vector via pointer arithmetic
            case VECTOR_ACTIVATION:{
                a->assembly[j] = (ins_t){
                    .instruction = ASM_ADD,
                    .destination = traverseContexts(a,quad.destination,NULL).data.address,
                    .source_A = traverseContexts(a,quad.source_A,NULL).data.address,
                    .source_B = traverseContexts(a,quad.source_B,NULL).data.address,
                };
                j++;
            }break;

            default:
                break;
        }

    }
    
    //layout constant memory
    a->memory = (size_t *)calloc(sizeof(size_t),a->varat);
    int bottom = 0;
    resetDict(&a->varmap);
    while (!bottom){
        entry_t variable = nextDict(&a->varmap,&bottom);
        if(variable.type == CONTEXT)
            continue;
        a->memory[variable.data.address] = variable.data.value;
    }
    a->memory[PC_ADDR] = queryDict(&a->varmap,"main",NULL,0,NULL).data.value;
    a->memory[ARG_STACK_ADDR] = a->progat + a->varat;
    a->memory[CALL_STACK_ADDR] = a->progat + a->varat + ARGUMENT_STACK_SIZE;
    a->memory[ONE_CT_ADDR] = 1;
    a->memory[THREE_CT_ADDR] = 3;
    writeBinary(a);
}

void destroyAssembler(assembler * a){
    
    if(!a->quiet){
        printf("VARIABLE MAP:\n");
        traverseDict(&a->varmap,0);
        printf("\nPMEM @ %lld DUPING VARMEM\n",a->varat);
        for(size_t i = 0; i < a->varat; i++)
            printf("%lld: %lld\n",i,a->memory[i]);
        printf("\nDUPING PMEM\n");
        
        char OPCODE_NAMES[16][32] = {"ASM_ADD","ASM_SUB","ASM_MUL","ASM_DIV","ASM_FJMP","ASM_LT","ASM_GT","ASM_LEQ","ASM_GEQ","ASM_EQ","ASM_NEQ","ASM_SET","ASM_SETDS","ASM_SETDD","ASM_SETDDI","ASM_SETI"};
        char RESERVED_NAMES[16][64] = {"PC","A_STACK","C_STACK","ONE","THREE","MISC_R","RET_R","DISP_ADDR","SWICH_ADDR","DEF_VEC_A","DEF_VEC_B"};
        
        for(size_t i = 0; i < a->progat; i++){
            printf("%lld: %s(%d)\t",i + a->varat, OPCODE_NAMES[a->assembly[i].instruction],a->assembly[i].instruction);
            size_t addresses[3] = {a->assembly[i].destination,a->assembly[i].source_A,a->assembly[i].source_B};
            for(size_t j = 0; j < 3; j++){
                if(j == 2 && ASM_NEQ < a->assembly[i].instruction)
                    continue;
                if(addresses[j] < CONSTANT_MEMORY_START)
                    printf("%s(%lld)\t",RESERVED_NAMES[addresses[j]],addresses[j]);
                else
                    printf("%lld\t",addresses[j]);
            }
            printf("\n");
        }
        printf("\n");
    }

    destroyIntermediary(a->inter);
    destroyDict(&a->varmap);
    destroyStack(&a->context);
    free(a->assembly);
    free(a->memory);
}