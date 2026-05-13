module circular #(
    parameter DEPTH = 256,
    parameter WIDTH = 16
)(
    input  wire clock,
    input  wire produce,
    input  wire [WIDTH-1:0] produced,
    input  wire consume,

    output reg  [WIDTH-1:0] consumed,
    output wire [$clog2(DEPTH)-1:0] capacity
);

    wire pedge;
    wire cedge;
    rising pdetector(.signal(produce), .clock(clock), .risen(pedge));
    rising cdetector(.signal(consume), .clock(clock), .risen(cedge));

    reg [$clog2(DEPTH)-1:0] write = 0;
    reg [$clog2(DEPTH)-1:0] read  = 0;
    reg [WIDTH-1:0] memory [0:DEPTH-1];

    wire empty = write == read;
    wire full = write + 1 == read;
    assign capacity = DEPTH - 1 - (write - read);

    always @(posedge clock) begin
        if (pedge && !full) begin
            memory[write] <= produced;
            write         <= write + 1;
        end
    end

    always @(posedge clock) begin 
        if (cedge && !empty) begin
            consumed <= memory[read];
            read     <= read + 1;
        end else if(cedge && empty)
            consumed <= 0;
    end

endmodule