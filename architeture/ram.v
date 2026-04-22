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
	
	integer i;

	initial begin
		$display("---- Loading RAM ----");
		$readmemh("ram.mem", ram);

		// Print first few locations
		for (i = 0; i < 39; i = i + 1) begin
			$display("[%0d]0x%h", i, ram[i]);
		end

		$display("---------------------");
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