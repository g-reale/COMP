`include "global.v"

module processor(
    
    `ifdef DEBUG
    output reg read_clock,
    output reg write_clock,
    output reg [`arg_l] read_from,
    output reg [`arg_l] write_into,
    output wire [`word_l] read,
    output reg [`word_l] write,
    output reg [`op_l] operator,
    output reg [`word_l] arg_a,
    output reg [`word_l] arg_b,
    output reg [`word_l] currpc,
    output wire [`word_l] nxtpc,
    output wire [`word_l] result,
    output reg [`arg_l] op_a,
    output reg [`arg_l] op_b,
    output reg [`arg_l] op_c,
    output reg [`word_l] displaying,
    output reg [4:0] state = INSTRUCTION_FETCH,
    output reg [4:0] goto = 0,
    output reg [`word_l]query,
    `endif 

    input wire CLOCK_50,
    input wire [17:0] SW,
	input wire [3:0] KEY,
    output wire [6:0] HEX0,
    output wire [6:0] HEX1,
    output wire [6:0] HEX2,
    output wire [6:0] HEX3,
    output wire [6:0] HEX4,
    output wire [6:0] HEX5,
    output wire [6:0] HEX6,
    output wire [6:0] HEX7

);

    // clock divider 
    `ifdef DEBUG
    wire clock;
    assign clock = CLOCK_50;

    `else
    wire clock;
    assign clock = CLOCK_50;
    
//    reg clock;
//    reg [25:0] divider = 26'd0;
//    always @(posedge CLOCK_50) begin
//        if(divider == 26'd25000000) begin //25000000
//            clock <= !clock;
//            divider <= 0;
//        end else begin
//            divider <= divider + 1;
//       end
//    end
        
    reg read_clock;
    reg write_clock;
    reg [`arg_l] read_from;
    reg [`arg_l] write_into;
    wire [`word_l] read;
    reg [`word_l] write;
    reg [`op_l] operator;
    reg [`word_l] arg_a;
    reg [`word_l] arg_b;
    reg [`word_l] currpc;
    wire [`word_l] nxtpc;
    wire [`word_l] result;
    reg [`arg_l] op_a;
    reg [`arg_l] op_b;
    reg [`arg_l] op_c;
    reg [`word_l] displaying;
    reg [4:0] state = INSTRUCTION_FETCH;
    reg [4:0] goto = 0;
    reg [`word_l]query;
    `endif

    ram r(
        .data(write),
        .q(read),
        .read_addr(read_from),
        .write_addr(write_into),
        .read_clock(read_clock),
        .write_clock(write_clock),
        .we(1)
    );

    alu a(
        .operator(operator),
        .arg_a(arg_a),
        .arg_b(arg_b),
        .currpc(currpc),
        .nxtpc(nxtpc),
        .result(result)
    );
	 
	 ssd d(
        .number(displaying),
        .clock(CLOCK_50),
        .hex0(HEX0),
        .hex1(HEX1),
        .hex2(HEX2),
        .hex3(HEX3),
        .hex4(HEX4),
        .hex5(HEX5),
        .hex6(HEX6),
        .hex7(HEX7)
	 );

    localparam INSTRUCTION_FETCH                      = 0;
    localparam INSTRUCTION_FETCH_1                    = 1;
    localparam INSTRUCTION_FETCH_2                    = 2;
    localparam INSTRUCTION_FETCH_3                    = 3;
    localparam ARITHMETIC                             = 4;
    localparam ARITHMETIC_1                           = 5;
    localparam SET_DEFERENCE_DESTINATION              = 6;
    localparam SET_DEFERENCE_DESTINATION_1            = 7;
    localparam SET_DEFERENCE_DESTINATION_IMMEDIATE    = 8;
    localparam SET_DEFERENCE_DESTINATION_IMMEDIATE_1  = 9;
    localparam SET_DEFERENCE_SOURCE                   = 10;
    localparam SET_DEFERENCE_SOURCE_1                 = 11;
    localparam SET                                    = 12;
    localparam DEFERENCE                              = 13;
    localparam DEFERENCE_1                            = 14;
	localparam DEFERENCE_2                            = 15;
    localparam WRITE                                  = 16;
    localparam WRITE_1                                = 17;
    localparam WRITE_BACK                             = 18;
    localparam WRITE_BACK_1                           = 19;
    localparam SWICH_READ                             = 20;


    always @(posedge clock) begin
        case(state)
            
			DEFERENCE: begin
				case(read_from)
					`SWITCH_ADDR: begin
						query <= SW[16:0];
						displaying <= SW[16:0];
						if(SW[17]) state <= SWICH_READ;
				    end
					 
					default: begin
						read_clock <= 0;
						state <= DEFERENCE_1;
					end
				endcase
            end

            DEFERENCE_1: begin
                read_clock <= 1;
                state <= DEFERENCE_2; 
            end
				
			DEFERENCE_2: begin
				query <= read;
				state <= goto; 
			end
			
			SWICH_READ: begin
				 displaying <= 0;
			    if(SW[17] == 0) state <= goto;
		    end

            WRITE: begin
                write_clock <= 0;
                state <= WRITE_1;
            end

            WRITE_1: begin
                write_clock <= 1;
                state <= goto;
            end

            WRITE_BACK: begin			
                case(write_into)
                    `PC_ADDR: begin
                        write <= result;
                        state <= WRITE;
                        goto <= INSTRUCTION_FETCH;
                    end
                    
                    `DISP_ADDR: begin
                        displaying <= result;
                        write <= result;
                        state <= WRITE;
                        goto	<= WRITE_BACK_1;
                    end
                    
                    default: begin
                        write <= result;
                        state <= WRITE;
                        goto	<= WRITE_BACK_1;
                    end
                endcase
            end

            WRITE_BACK_1: begin
                write <= nxtpc;
                write_into <= `PC_ADDR;
                state <= WRITE;
                goto <= INSTRUCTION_FETCH;
            end

            INSTRUCTION_FETCH: begin
                read_from <= `PC_ADDR;
                state <= DEFERENCE;
                goto <= INSTRUCTION_FETCH_1;
            end

            INSTRUCTION_FETCH_1: begin
                currpc <= query;
                read_from <= query;
                state <= DEFERENCE;
                goto <= INSTRUCTION_FETCH_2;
            end

            INSTRUCTION_FETCH_2: begin
                operator    <= query[`WORD_LENGTH-1 : `WORD_LENGTH-`OP_LENGTH];
                op_c        <= query[`WORD_LENGTH-`OP_LENGTH-1 : `WORD_LENGTH-`OP_LENGTH-`ARG_LENGTH];
                op_a        <= query[`WORD_LENGTH-`OP_LENGTH-`ARG_LENGTH-1 : `WORD_LENGTH-`OP_LENGTH-(2*`ARG_LENGTH)];
                op_b        <= query[`WORD_LENGTH-`OP_LENGTH-(2*`ARG_LENGTH)-1 : `WORD_LENGTH-`OP_LENGTH-(3*`ARG_LENGTH)];
                state       <= INSTRUCTION_FETCH_3;
            end

            INSTRUCTION_FETCH_3: begin

                case(operator)
                    `ADD, `SUB, `MUL, `DIV, `LT, `GT, `LEQ, `GEQ, `EQ, `NEQ, `JMP: begin
                        read_from <= op_a;
                        state <= DEFERENCE;
                        goto <= ARITHMETIC;
                    end

                    `SETDD: begin
                        read_from <= op_a;
                        state <= DEFERENCE;
                        goto <= SET_DEFERENCE_DESTINATION;
                    end

                    `SETDDI: begin
                        arg_a <= op_a;
                        state <= SET_DEFERENCE_DESTINATION_IMMEDIATE;
                    end

                    `SETDS: begin
                        read_from <= op_a;
                        state <= DEFERENCE;
                        goto <= SET_DEFERENCE_SOURCE;
                    end

                    `SET: begin
                        read_from <= op_a;
                        state <= DEFERENCE;
                        goto <= SET;
                    end

                    `SETI: begin
                        arg_a <= op_a;
                        write_into <= op_c;
                        state <= WRITE_BACK;
                    end

                endcase
            end

            ARITHMETIC: begin
                arg_a <= query;
                read_from <= op_b;
                state <= DEFERENCE;
                goto <= ARITHMETIC_1;
            end

            ARITHMETIC_1: begin
                arg_b <= query;
                write_into <= op_c;
                state <= WRITE_BACK;
            end

            SET_DEFERENCE_DESTINATION: begin
                arg_a <= query;
                read_from <= op_c;
                state <= DEFERENCE;
                goto <= SET_DEFERENCE_DESTINATION_1;
            end

            SET_DEFERENCE_DESTINATION_1: begin
                write_into <= query;
                state <= WRITE_BACK;
            end

            SET_DEFERENCE_DESTINATION_IMMEDIATE: begin
                read_from <= op_c;
                state <= DEFERENCE;
                goto <= SET_DEFERENCE_DESTINATION_IMMEDIATE_1;
            end

            SET_DEFERENCE_DESTINATION_IMMEDIATE_1: begin
                write_into <= query;
                state <= WRITE_BACK;
            end

            SET_DEFERENCE_SOURCE: begin
                read_from <= query;
                state <= DEFERENCE;
                goto <= SET_DEFERENCE_SOURCE_1;
            end

            SET_DEFERENCE_SOURCE_1: begin
                arg_a <= query;
                write_into <= op_c;
                state <= WRITE_BACK;
            end

            SET: begin
                arg_a <= query;
                write_into <= op_c;
                state <= WRITE_BACK;
            end

            default: state <= INSTRUCTION_FETCH;
        endcase
    end
endmodule