`include "global.v"

// Quartus Prime Verilog Template
// Simple Dual Port RAM with separate read/write addresses and
// separate read/write clocks

module ram
#(parameter DATA_WIDTH=`WORD_LENGTH, parameter ADDR_WIDTH=`ARG_LENGTH)
(
	input [(DATA_WIDTH-1):0] data,
	input [(ADDR_WIDTH-1):0] read_addr, write_addr,
	input we, read_clock, write_clock,
	output reg [(DATA_WIDTH-1):0] q
);

	// Declare the RAM variable
	reg [DATA_WIDTH-1:0] ram[2**ADDR_WIDTH-1:0];
	
	initial begin
	ram[0] = 34'b0000000000000000000000000000000110;
        ram[1] = 34'b0000000000000000000000000000000000;
        ram[2] = 34'b0000000000000000000000000000000000;
        ram[3] = 34'b0000000000000000000000000000000000;
        ram[4] = 34'b0000000000000000000000000000001011;
        ram[5] = 34'b0000000000000000000000000000000110;
        ram[6] = 34'b1100000000000100000100100000000000;
        ram[7] = 34'b1100000000001000000100100000000000;
        ram[8] = 34'b0101000000001100000000010000000010;
        ram[9] = 34'b1010100000000000000000110000000100;
        ram[10] = 34'b1100000001001100000000100000000000;
        ram[11] = 34'b1100000000000000000001010000000000;
        ram[12] = 34'b1100000001001100000000010000000000;
        ram[13] = 34'b1100000000000000000001010000000000;
	end
	
	always @ (posedge write_clock)
	begin
		// Write
		if (we)
			ram[write_addr] <= data;
	end
	
	always @ (posedge read_clock)
	begin
		// Read 
		q <= ram[read_addr];
	end
	
endmodule
