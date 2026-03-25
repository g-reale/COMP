`include "global.v"

module audio #(
    parameter DEPTH = 256
)
(
    input wire clock,
    input wire produce,
    input wire [`word_l] produced,
    
    output wire [`word_l] capacity,
    output wire mclk,
    output wire bclk,
    output wire daclrc,
    output reg dacdat
);
    
    localparam WIDTH = 16;
    reg consume = 0;
    wire [WIDTH-1:0] consumed;

    circular #(.DEPTH(DEPTH), .WIDTH(WIDTH)) samples(
        .clock(clock),
        .produce(produce),
        .produced(produced[WIDTH-1:0] ^ (16'b1 << (WIDTH -1))),
        .consume(consume),
        .consumed(consumed),
        .capacity(capacity[$clog2(DEPTH)-1:0])
    );

    reg [9:0] counter = 0;

    always @(posedge clock)
        counter <= counter + 1;

    assign mclk   = ~counter[1]; //MAYBE INVERTED I DON'T KNOW
    assign bclk   = counter[3];
    assign daclrc = counter[9];

    wire [9:0] next_count = counter + 2;
    wire bclk_fall        = next_count[3] < counter[3];
    wire daclrc_change    = next_count[9] != counter[9];

    localparam RESET    = 0;
    localparam RESET_1  = 1;
    localparam RESET_2  = 2;
    localparam SEND     = 3;
    localparam SEND_1   = 4;
    localparam PAD      = 5;
    localparam PAD_1    = 6;

    reg [WIDTH-1:0] sample = 0;
    reg [2:0] state = RESET; //SETUP_DATA
    reg [4:0] index;
    
    always @(posedge clock) begin
        case (state)
            
            RESET: begin
                index   <= 0;
                consume <= 1;
                state   <= RESET_1;
            end

            RESET_1: begin
                sample  <= consumed;
                consume <= 0;
                state   <= RESET_2;
            end

            RESET_2: begin
                state <= daclrc_change ? SEND : RESET_2;
            end

            SEND: begin
                dacdat <= sample[WIDTH-1];
                sample <= sample << 1;
                index  <= index + 1;
                state  <= SEND_1;
            end

            SEND_1: begin
                state <= bclk_fall ? (index == 16 ? PAD : SEND) : SEND_1;
            end

            PAD: begin
                dacdat <= 0;
                index  <= index + 1;
                state  <= PAD_1;
            end

            PAD_1: begin
                state <= bclk_fall ? (index == 32 ? PAD : RESET) : PAD_1;
            end

        endcase
    end

endmodule