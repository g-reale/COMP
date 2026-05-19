module processor(
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
    output wire [6:0] HEX7,
    output wire [7:0]LCD_DATA,
    output wire LCD_RS,
    output wire LCD_EN,
    output wire LCD_RW,
    output wire MCLK,
    output wire BCLK,
    output wire DACLRC,
    output wire DACDAT
);

    
    reg read_clock = 0;
    reg write_clock = 0;
    reg [`arg_l] read_from = 0;
    reg [`arg_l] write_into = 0;
    wire [`word_l] read;
    reg [`word_l] write = 0;
    reg [`op_l] operator = 0;
    reg [`word_l] arg_a = 0;
    reg [`word_l] arg_b = 0;
    reg [`word_l] currpc = 0;
    wire [`word_l] nxtpc;
    wire [`word_l] result;
    reg [`arg_l] op_a = 0;
    reg [`arg_l] op_b = 0;
    reg [`arg_l] op_c = 0;
    reg [`word_l] displaying = 0;
    reg [4:0] state = INSTRUCTION_FETCH;
    reg [4:0] goto = 0;
    reg [`word_l] query = 0;
    reg [7:0] character = 0;
    wire ready;
    reg consume = 0;
    reg interruption = 0;
    reg [`word_l] quantum = 0;
    reg [`word_l] destination = 0;
    wire [`word_l] capacity;
    reg [`word_l] sample = 0;
    reg produce = 0;
    wire [17:0] switches;
    

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

     lcd l(
        .ascii(character),
        .clock(CLOCK_50),
        .ready(ready),
        .consume(consume),
        .lcd_rs(LCD_RS),
        .lcd_en(LCD_EN),
        .lcd_rw(LCD_RW),
        .lcd_data(LCD_DATA)
     );

     audio u(
        .clock(CLOCK_50),
        .produce(produce),
        .produced(sample),
        .capacity(capacity),
        .mclk(MCLK),
        .bclk(BCLK),
        .daclrc(DACLRC),
        .dacdat(DACDAT)
     );

     debounce db(
        .clock(CLOCK_50),
        .switches(SW),
        .debounced(switches)
     );

    localparam INSTRUCTION_FETCH                      = 5'd0;
    localparam INSTRUCTION_FETCH_1                    = 5'd1;
    localparam INSTRUCTION_FETCH_2                    = 5'd2;
    localparam INSTRUCTION_FETCH_3                    = 5'd3;
    localparam ARITHMETIC                             = 5'd4;
    localparam ARITHMETIC_1                           = 5'd5;
    localparam SET_DEFERENCE_DESTINATION              = 5'd6;
    localparam SET_DEFERENCE_DESTINATION_1            = 5'd7;
    localparam SET_DEFERENCE_DESTINATION_IMMEDIATE    = 5'd8;
    localparam SET_DEFERENCE_DESTINATION_IMMEDIATE_1  = 5'd9;
    localparam SET_DEFERENCE_SOURCE                   = 5'd10;
    localparam SET_DEFERENCE_SOURCE_1                 = 5'd11;
    localparam SET                                    = 5'd12;
    localparam DEFERENCE                              = 5'd13;
    localparam DEFERENCE_1                            = 5'd14;
    localparam DEFERENCE_2                            = 5'd15;
    localparam WRITE                                  = 5'd16;
    localparam WRITE_1                                = 5'd17;
    localparam WRITE_BACK                             = 5'd18;
    localparam WRITE_BACK_1                           = 5'd19;
    localparam SWICH_READ                             = 5'd20;
    localparam LCD                                    = 5'd21;
    localparam LCD_1                                  = 5'd22;
    localparam INTERRUPTION_START                     = 5'd23;
    localparam INTERRUPTION_START_1                   = 5'd24;
    localparam INTERRUPTION_END                       = 5'd25;
    localparam INTERRUPTION_END_1                     = 5'd26;
    localparam INTERRUPTION_END_2                     = 5'd27;
    localparam AUDIO_WRITE                            = 5'd28;

    always @(posedge CLOCK_50) begin
        
       if (!KEY[0]) begin
       state      <= INSTRUCTION_FETCH;
       currpc     <= 0;
       query      <= 0;
       operator   <= 0;
       arg_a      <= 0;
       arg_b      <= 0;
       read_from  <= 0;
       write_into <= 0;
       produce    <= 0;
       interruption <= 0;
       quantum <= 0;
       end else begin
        
        case(state)
            
			DEFERENCE: begin
				case(read_from)
					`SWITCH_ADDR: begin
						query <= switches[16:0];
						if(switches[16:0] != 0) displaying <= switches[16:0];
						if(switches[17]) state <= SWICH_READ;
				    end
					 
					  `AUDIO_CAPACITY: begin
							query <= capacity;
							state <= goto;
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
			    if(switches[17] == 0) state <= goto;
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
                        goto  <= WRITE_BACK_1;
                    end

                    `LCD_ADDR: begin
                        state <= ready ? LCD : WRITE_BACK;
                    end

                    `QUANTUM: begin
                        if (result) begin
                            interruption <= 1;
                            quantum <= result;
                            read_from <= `DESTINATION;
                            state <= DEFERENCE;
                            goto <= INTERRUPTION_START;
                        end else begin
                            interruption <= 0;
                            quantum <= 0;
                            state <= WRITE_BACK_1;
                        end
                    end

                    `AUDIO_SINK: begin
                        sample <= result;
                        produce <= 0;
                        state <= AUDIO_WRITE;
                    end

                    default: begin
                        // if (write_into < `ROM_START) begin // RAM
                            write <= result;
                            state <= WRITE;
                            goto  <= WRITE_BACK_1;
                        // end else begin // ROM
                        //     state <= WRITE_BACK_1;
                        // end
                    end
                endcase
            end

            AUDIO_WRITE: begin
                produce <= 1;
                state <= WRITE;
                goto <= WRITE_BACK_1;
            end

            INTERRUPTION_START: begin
                write_into <= `PC_ADDR;
                write <= query;
                state <= WRITE;
                goto <= INTERRUPTION_START_1;
            end

            INTERRUPTION_START_1: begin
                write_into <= `DESTINATION;
                write <= nxtpc;
                state <= WRITE;
                goto <= INSTRUCTION_FETCH;
            end

            WRITE_BACK_1: begin
                write <= nxtpc;
                write_into <= `PC_ADDR;
                state <= WRITE;
                goto <= INSTRUCTION_FETCH;
            end

            LCD: begin
                consume <= 1;
                character <= result;
                state <= ready ? LCD : LCD_1;
            end
            
            LCD_1: begin
                consume <= 0;
                write <= result;
                state <= WRITE;
                goto  <= WRITE_BACK_1;
            end

            INSTRUCTION_FETCH: begin
                if((interruption && quantum) || (!interruption)) begin
                    quantum <= quantum - interruption;
                    read_from <= `PC_ADDR;
                    state <= DEFERENCE;
                    goto <= INSTRUCTION_FETCH_1;
                end else begin
                    interruption <= 0;
                    quantum <= 0;
                    read_from <= `DESTINATION;
                    state <= DEFERENCE;
                    goto <= INTERRUPTION_END;
                end
            end

            INTERRUPTION_END: begin
                destination <= query;
                read_from <= `PC_ADDR;
                state <= DEFERENCE;
                goto <= INTERRUPTION_END_1;
            end
            
            INTERRUPTION_END_1: begin
                write_into <= `DESTINATION;
                write <= query;
                state <= WRITE;
                goto <= INTERRUPTION_END_2;
            end
            
            INTERRUPTION_END_2: begin
                write_into <= `PC_ADDR;
                write <= destination;
                state <= WRITE;
                goto <= INSTRUCTION_FETCH;
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
                    `ADD, `SUB, `MUL, `DIV, `LT, `GT, `LEQ, `GEQ, `EQ, `NEQ, `FJMP: begin
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
    end
endmodule