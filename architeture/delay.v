module delay #(
    parameter DELAY = 1
)(
    input  wire signal,
    input  wire clock,
    output wire delayed
);
    reg [DELAY:0] shift = 0;

    always @(posedge clock)
        shift <= (signal << DELAY) | (shift >> 1);

    assign delayed = shift[0];

endmodule