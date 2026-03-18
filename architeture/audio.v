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

    wire clk12Mhz = counter[1];
    wire clk3Mhz  = counter[3];
    wire clk48Khz = counter[9];

    //manually configured delays to align the waves as required by the codec
    delay #(.DELAY(2)) mdelay (.signal(clk12Mhz), .clock(clock), .delayed(mclk));
    delay #(.DELAY(8)) bdelay (.signal(clk3Mhz),  .clock(clock), .delayed(bclk));
    delay #(.DELAY(24)) lrdelay(.signal(clk48Khz), .clock(clock), .delayed(daclrc));

    localparam SEND_DATA     = 0;
    localparam SETUP_PADDING = 1;
    localparam SEND_PADDING  = 2;
    localparam SETUP_DATA    = 3;

    reg [WIDTH-1:0] sample = 0;
    reg [1:0] state = SETUP_DATA;
    reg [$clog2(WIDTH)-1:0] index = 0;
    wire bedge;
    rising bdetector(.signal(clk3Mhz), .clock(clock), .risen(bedge));

    always @(posedge clock) begin
        if(bedge) begin
            case (state)
                
                SEND_DATA: begin
                    dacdat <= sample[WIDTH-1];
                    sample <= sample << 1;
                    index  <= index + 1;
                    state  <= index == (WIDTH-2) ? SETUP_PADDING : SEND_DATA;
                end

                SETUP_PADDING: begin
                    dacdat  <= sample[WIDTH-1];
                    sample  <= sample << 1;
                    index   <= 0;
                    consume <= 1;
                    state   <= SEND_PADDING;
                end

                SEND_PADDING: begin
                    dacdat <= 0;
                    index  <= index + 1;
                    state  <= index == (WIDTH-2) ? SETUP_DATA : SEND_PADDING;
                end

                SETUP_DATA: begin
                    dacdat  <= 0;
                    index   <= 0;
                    consume <= 0;
                    sample  <= consumed;
                    state   <= SEND_DATA;
                end

            endcase
        end
    end

endmodule