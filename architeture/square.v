`include "global.v"

module square #(
    parameter HALF_PERIOD = 48000
)
(
    input wire CLOCK_50,
    output wire MCLK,
    output wire BCLK,
    output wire DACLRC,
    output wire DACDAT
);

    wire [`word_l] capacity;
    reg [`word_l] wave = 0;
    reg produce = 0;
    
    audio #(.DEPTH(512)) a(
        .clock(CLOCK_50),
        .produce(produce),
        .produced(wave),
        .capacity(capacity),
        .mclk(MCLK),
        .bclk(BCLK),
        .daclrc(DACLRC),
        .dacdat(DACDAT)
    );

    localparam SEND   = 0;
    localparam INVERT = 1;
    localparam RESET  = 2;

    reg [1:0] state = SEND;
    reg [18:0] sent = 0;

    always @(posedge CLOCK_50) begin
        if(capacity) begin
        case(state)    
            
            SEND: begin
                produce <= 1;
                sent <= sent + 1;
                state <= sent == HALF_PERIOD ? INVERT : RESET;
            end

            INVERT: begin
                wave <= ~wave;
                produce <= 0;
                sent <= 0;
                state <=SEND;
            end

            RESET: begin
                produce <= 0;
                state <= SEND;
            end

        endcase
    end
    end
endmodule