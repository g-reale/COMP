`include "global.v"

module testbench();
    reg clk;
    processor proc(clk);
    
    initial begin
        clk = 0;
        forever #10 clk = ~clk; // Toggle clock every 5 time units
    end

    initial begin
        // Monitor RAM values
        $monitor("time=%4d, RAM[2]=%d, RAM[3]=%d", 
                $time,
                proc.datamem.ram[2], 
                proc.datamem.ram[3]);
        #2000;
        $finish;
    end


endmodule