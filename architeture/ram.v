`include "global.v"

module ram
(
	input [`word_l] result,
	input [`arg_l] pointer_a,pointer_b, pointer_w,
	input clk,
	output [`word_l] data_a,data_b
);

	reg [`word_l] ram[`mem_s];

    initial begin
        $readmemb("ram.mem", ram);
        ram[0] = 0; //always keep a zero and a one in the ram start
        ram[1] = 1; 
    end

	always @ (negedge clk)
	begin
        if(1 < pointer_w)
			ram[pointer_w] <= result;
	end

    assign data_a = ram[pointer_a];
    assign data_b = ram[pointer_b];

endmodule
