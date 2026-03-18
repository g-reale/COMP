module rising(
    input  wire signal,
    input  wire clock,
    output wire risen
);
    reg delayed = 0;
    assign risen = signal > delayed;

    always @(posedge clock) begin
        delayed <= signal;
    end

endmodule