`include "global.v"
`timescale 1ns/1ps

module audio_tb;

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
        $dumpfile("audio_tb.vcd");
        $dumpvars(0, audio_tb);
        #(50_000_000);
        $finish;
    end

endmodule

// verilog -o audio_tb.vvp audio_tb.v audio.v circular.v rising.v delay.v global.v square.v && vvp audiotest_tb.vvp && gtkwave audiotest_tb.vcd