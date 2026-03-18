`include "global.v"
`timescale 1ns/1ps

module audiotest_tb;

    reg CLOCK_50 = 0;
    always #10 CLOCK_50 = ~CLOCK_50;

    wire MCLK, BCLK, DACLRC, DACDAT;

    square dut(
        .CLOCK_50(CLOCK_50),
        .MCLK(MCLK),
        .BCLK(BCLK),
        .DACLRC(DACLRC),
        .DACDAT(DACDAT)
    );

    initial begin
        $dumpfile("audiotest_tb.vcd");
        $dumpvars(0, audiotest_tb);
        // #(50_000_000 * 20); // 1 second of simulation
        #(50_000_000);
        $finish;
    end

endmodule