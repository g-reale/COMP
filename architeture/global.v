//ALU INSTRUCTIONS
`define ADD 0
`define SUB 1
`define MUL 2
`define DIV 3
`define AND 4
`define OR  5
`define NOR 6
`define XOR 7
`define SFL 8 
`define SFR 9
`define LT  10
`define GT  11
`define EQ  12
`define JMP 13

`define WORD_LENGTH 64
`define OP_LENGTH 4
`define ARG_LENGTH 20 // (64 - 4)/3
`define MEM_SIZE 1048576 //distribute remaining bits between 3 operands (2^20)

`define word_l 63:0
`define op_l 3:0
`define arg_l 19:0
`define mem_s 1048575:0