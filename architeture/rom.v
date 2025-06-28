`include "global.v"

module rom
(
	input [`word_l] pointer,
	input clk, 
	output reg [`word_l] instruction
);

	reg [`word_l] rom[`mem_s];

	initial
	begin
		$readmemb("rom.mem", rom);
	end

	always @ (posedge clk)
	begin
		instruction <= rom[pointer];
	end

endmodule
