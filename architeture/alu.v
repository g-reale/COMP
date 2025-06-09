`include "global.v"

module alu(
    input  wire [`op_l] operator,
    input  wire [`word_l] arg_a,
    input  wire [`word_l] arg_b,
    input  wire [`word_l] currpc,
    output reg  [`word_l] nxtpc,
    output reg  [`word_l] result
);

    always @* begin
        case (operator)
            `ADD:  result = arg_a + arg_b;
            `SUB:  result = arg_a - arg_b;
            `MUL:  result = arg_a * arg_b;
            `DIV:  result = arg_b != 0 ? arg_a / arg_b : 16'b0;
            `AND:  result = arg_a & arg_b;
            `OR:   result = arg_a | arg_b;
            `NOR:  result = ~(arg_a | arg_b);
            `XOR:  result = arg_a ^ arg_b;
            `SFL:  result = arg_a << arg_b;
            `SFR:  result = arg_a >> arg_b;
            `LT:   result = (arg_a < arg_b);
            `GT:   result = (arg_a > arg_b);
            `EQ:   result = (arg_a == arg_b);
        endcase

        if (operator == `JMP && arg_a != 0) begin
            nxtpc = arg_b;
            result = currpc;
        end else
            nxtpc = currpc + 1;
    end



endmodule