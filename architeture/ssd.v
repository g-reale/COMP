module ssd(
    input  wire [31:0] number,
    output wire [6:0] hex0,
    output wire [6:0] hex1,
    output wire [6:0] hex2,
    output wire [6:0] hex3,
    output wire [6:0] hex4,
    output wire [6:0] hex5,
    output wire [6:0] hex6,
    output wire [6:0] hex7
);

    wire [6:0] displays [7:0];
    assign hex0 = displays[0];
    assign hex1 = displays[1];
    assign hex2 = displays[2];
    assign hex3 = displays[3];
    assign hex4 = displays[4];
    assign hex5 = displays[5];
    assign hex6 = displays[6];
    assign hex7 = displays[7];
    
    genvar i;
    generate
        for (i = 0; i < 8; i = i + 1) begin : gen_displays
            wire [3:0] digit;
            assign digit = (number / 10**i) % 10;
            assign displays[i] = (
                 (digit == 4'd9) ? 7'b0001100 :
                 (digit == 4'd8) ? 7'b0000000 :
                 (digit == 4'd7) ? 7'b0001111 :
                 (digit == 4'd6) ? 7'b0100000 :
                 (digit == 4'd5) ? 7'b0100100 :
                 (digit == 4'd4) ? 7'b1001100 :
                 (digit == 4'd3) ? 7'b0000110 :
                 (digit == 4'd2) ? 7'b0010010 :
                 (digit == 4'd1) ? 7'b1001111 :
                 (digit == 4'd0) ? 7'b0000001 :
                 7'b1111111
            );
    end
endgenerate

endmodule
