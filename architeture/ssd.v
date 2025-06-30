`include "global.v"

module ssd(
    input  wire [31:0] number,
    input  wire clock,
    output wire [6:0] hex0,
    output wire [6:0] hex1,
    output wire [6:0] hex2,
    output wire [6:0] hex3,
    output wire [6:0] hex4,
    output wire [6:0] hex5,
    output wire [6:0] hex6,
    output wire [6:0] hex7
);

    localparam FETCH_NUMBER = 0;
    localparam DISPLAY      = 1;
    localparam UPDATE       = 2;

    function [6:0] encode;
        input [3:0] digit;
        begin
            case(digit)
                4'd0: encode = 7'b1000000;
                4'd1: encode = 7'b1111001;
                4'd2: encode = 7'b0100100;
                4'd3: encode = 7'b0110000;
                4'd4: encode = 7'b0011001;
                4'd5: encode = 7'b0010010;
                4'd6: encode = 7'b0000010;
                4'd7: encode = 7'b1111000;
                4'd8: encode = 7'b0000000;
                4'd9: encode = 7'b0010000;
                default: encode = 7'b1111111;
            endcase
        end
    endfunction

    reg [6:0] displays [7:0];
    initial begin
        displays[0] = 7'b1111111;
        displays[1] = 7'b1111111;
        displays[2] = 7'b1111111;
        displays[3] = 7'b1111111;
        displays[4] = 7'b1111111;
        displays[5] = 7'b1111111;
        displays[6] = 7'b1111111;
        displays[7] = 7'b1111111;
    end
    
    assign hex0 = displays[0];
    assign hex1 = displays[1];
    assign hex2 = displays[2];
    assign hex3 = displays[3];
    assign hex4 = displays[4];
    assign hex5 = displays[5];
    assign hex6 = displays[6];
    assign hex7 = displays[7];

    reg [1:0] state = FETCH_NUMBER;
    reg [31:0] displaying = 0;
    reg [2:0] focus = 0;

    always @(posedge clock) begin
        case (state)
            FETCH_NUMBER: begin
                displaying <= number;
                focus <= 0;
                state <= DISPLAY;
            end

            DISPLAY: begin
                displays[focus] <= encode(displaying % 10);
					 
					 case (focus)
						7: state <= FETCH_NUMBER;
						default: state <= UPDATE;
					 endcase
            end

            UPDATE: begin
                displaying <= displaying / 10;
                focus <= focus + 1;
					 state <= DISPLAY;
            end
        endcase
    end

endmodule
