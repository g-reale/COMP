`define ADD     0
`define SUB     1
`define MUL     2
`define DIV     3
`define FJMP    4
`define LT      5
`define GT      6
`define LEQ     7
`define GEQ     8
`define EQ      9
`define NEQ     10

`define SET     11 // ram[a] = ram[b]
`define SETDS   12 // ram[a] = ram[ram[b]]
`define SETDD   13 // ram[ram[a]] = ram[b]
`define SETDDI  14 // ram[ram[a]] = b
`define SETI    15 // ram[a] = b

`define WORD_LENGTH 37
`define OP_LENGTH   4
`define ARG_LENGTH  11   // (34 - 4)/3
`define MEM_SIZE    2048 // 2^11

`define word_l      36:0
`define op_l        3:0
`define arg_l       10:0
`define mem_s       2047:0

`define PC_ADDR 0
`define DISP_ADDR 7
`define SWITCH_ADDR 8
`define LCD_ADDR 9
`define QUANTUM 10
`define DESTINATION 11
`define AUDIO_SINK 13
`define AUDIO_CAPACITY 14

`define ROM_START 1792
`define AUDIO_BUFFER 512
//`define DEBUG 1