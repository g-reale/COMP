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
		ram[0] = 34'b0000000000000000000000000000000110;  // PC = 6
        ram[1] = 34'b0000000000000000000000000000000000;  // 1 = 0
        ram[2] = 34'b0000000000000000000000000000000000;  // 2 = 0
        ram[3] = 34'b0000000000000000000000000000000000;  // 3 = 0
        ram[4] = 34'b0000000000000000000000000000001100;  // 4 = 12
        ram[5] = 34'b0000000000000000000000000000001101;  // 5 = 13
		  
        ram[6] = 34'b1100000000000100000100100000000000;  //SET 1 SWICHES 
        ram[7] = 34'b1100000000001000000100100000000000;  //SET 2 SWICHES
        ram[8] = 34'b0101000000001100000000010000000010;  //GT 3 2 1
        ram[9] = 34'b1010100000000000000000110000000100;  //JMP 512 3 4
        ram[10] = 34'b1100000001001100000000100000000000; //SET DISPLAY 1
        ram[11] = 34'b1100000000000000000001010000000000; //SET PC 5
        ram[12] = 34'b1100000001001100000000010000000000; //SET DISPLAY 2
        ram[13] = 34'b1100000000000000000001010000000000; //SET PC 5
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
