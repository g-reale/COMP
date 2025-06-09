`include "global.v"

module processor(
    input clk
);
    reg [`word_l] pc;
    wire [`word_l] nxtpc;
    wire [`word_l] instruction;
    
    initial begin
        pc = 0;
    end

     always @(posedge clk) begin
        pc <= nxtpc;
    end

    rom progmem(
        .pointer(pc),
        .clk(clk),
        .instruction(instruction)
    );

    wire [`op_l] operator; 
    wire [`arg_l] destination, pointer_a, pointer_b;

    assign operator = instruction[63:60];
    assign destination = instruction[59:40];
    assign pointer_a = instruction[39:20];
    assign pointer_b = instruction[19:0];

    wire [`word_l] arg_a;
    wire [`word_l] arg_b;
    wire [`word_l] result;
    ram datamem(
        .result(result),
        .pointer_a(pointer_a),
        .pointer_b(pointer_b),
        .pointer_w(destination),
        .clk(clk),
        .data_a(arg_a),
        .data_b(arg_b)
    );

    alu calc(
        .operator(operator),
        .arg_a(arg_a),
        .arg_b(arg_b),
        .currpc(pc),
        .nxtpc(nxtpc),
        .result(result)
    );

endmodule