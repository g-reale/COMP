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
        result = 0;
        nxtpc = currpc + 1;

        case (operator)
            `ADD:   result = arg_a + arg_b;
            `SUB:   result = arg_a - arg_b;
            `MUL:   result = arg_a * arg_b;
            `DIV:   result = (arg_b != 0) ? arg_a / arg_b : 0;
            `LT:    result = (arg_a < arg_b);
            `GT:    result = (arg_a > arg_b);
            `LEQ:   result = (arg_a <= arg_b);
            `GEQ:   result = (arg_a >= arg_b);
            `EQ:    result = (arg_a == arg_b);
            `NEQ:   result = (arg_a != arg_b);

            `SET,
            `SETDS,
            `SETDDI,
            `SETDD: result = arg_a;

            `JMP: begin
                result = arg_a != 0 ? currpc : 0;
                nxtpc = (arg_a != 0) ? arg_b : currpc + 1;
            end
        endcase
    end
endmodule
