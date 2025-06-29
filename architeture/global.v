//ALU INSTRUCTIONS
`define ADD 0
`define SUB 1
`define MUL 2
`define DIV 3
`define LT  4
`define GT  5
`define LEQ 6
`define GEQ 7
`define EQ  8
`define NEQ 9
`define JMP  10 //if(ram[a]) ram[c] = pc; goto ram[b]

`define SET 12    // ram[a] = ram[b]
`define SETDS 13  // ram[a] = ram[ram[b]]
`define SETDD 14  // ram[ram[a]] = ram[b]
`define SETDDI 15 // ram[ram[a]] = b

`define WORD_LENGTH 34
`define OP_LENGTH 4
`define ARG_LENGTH 10 // (34 - 4)/3
`define MEM_SIZE 1024 // 2^10

`define word_l 33:0
`define op_l 3:0
`define arg_l 9:0
`define mem_s 1023:0

`define PC_ADDR 0
`define SWITCH_ADDR 18
`define DISP_ADDR 19

`define DEBUG 1