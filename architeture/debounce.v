module debounce #(
    parameter DEPTH = 18,
    parameter DELAY = 4545
)
(
    input wire clock,
    input wire [DEPTH-1:0] switches,
    output reg [DEPTH-1:0] debounced
);

    localparam WAIT = 0;
    localparam STABILIZE = 1;
    localparam OUTPUT = 2;

    integer counter = 0;
    reg [2:0] state = WAIT;
    
    always @(posedge clock) begin
        case(state)
            
            WAIT: begin
                state <= debounced != switches ? STABILIZE : WAIT;
                counter <= 0;
            end

            STABILIZE: begin
                state <= DELAY <= counter ? OUTPUT : STABILIZE;
                counter <= counter + 1;
            end

            OUTPUT: begin
                debounced <= switches;
                state <= WAIT;
            end

        endcase
    end
    
endmodule