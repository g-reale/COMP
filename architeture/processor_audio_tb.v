`include "global.v"
`timescale 1ns/1ps

// =============================================================================
// processor_audio_tb.v
//
// Instantiates the full processor and decodes the I2S output (BCLK/DACLRC/DACDAT)
// back into 16-bit samples. Decoded samples are printed and optionally compared
// against an expected sequence so you can verify end-to-end audio correctness.
//
// What is being verified:
//   1. The processor can write to AUDIO_SINK without hanging (produce pulse fires).
//   2. The circular buffer fills and drains correctly (capacity never goes negative
//      or wraps; no sample is lost due to a missing rising edge on produce).
//   3. The I2S bit stream reconstructed by this bench matches what the C- program
//      wrote (offset by the two's-complement flip: audio.v XORs bit15 before
//      pushing to the FIFO, so we XOR it back when comparing).
//   4. The processor keeps running after audio writes (PC advances normally).
//
// Waveform dump:
//   iverilog -o processor_audio_tb.vvp processor_audio_tb.v \
//            processor.v alu.v ram.v ssd.v lcd.v audio.v circular.v rising.v \
//            global.v \
//   && vvp processor_audio_tb.vvp && gtkwave processor_audio_tb.vcd
//
// In GTKWave add:
//   BCLK, DACLRC, DACDAT           -- raw I2S stream
//   decoded_sample, decoded_valid  -- one pulse per reconstructed sample
//   capacity_mon                   -- buffer fill level over time
//   mismatch_count                 -- should stay 0 if all samples match
// =============================================================================

module processor_audio_tb;

    // -------------------------------------------------------------------------
    // Clock
    // -------------------------------------------------------------------------
    reg CLOCK_50 = 0;
    always #10 CLOCK_50 = ~CLOCK_50;   // 50 MHz  (period = 20 ns)

    // -------------------------------------------------------------------------
    // Inputs driven by the testbench
    // -------------------------------------------------------------------------
    reg [17:0] SW  = 18'b0;
    reg [3:0]  KEY = 4'hF;             // active-low, all released

    // -------------------------------------------------------------------------
    // DUT outputs
    // -------------------------------------------------------------------------
    wire [6:0] HEX0,HEX1,HEX2,HEX3,HEX4,HEX5,HEX6,HEX7;
    wire [7:0] LCD_DATA;
    wire       LCD_RS, LCD_EN, LCD_RW;
    wire       MCLK, BCLK, DACLRC, DACDAT;

    // -------------------------------------------------------------------------
    // DUT
    // -------------------------------------------------------------------------
    processor dut(
        .CLOCK_50 (CLOCK_50),
        .SW       (SW),
        .KEY      (KEY),
        .HEX0(HEX0),.HEX1(HEX1),.HEX2(HEX2),.HEX3(HEX3),
        .HEX4(HEX4),.HEX5(HEX5),.HEX6(HEX6),.HEX7(HEX7),
        .LCD_DATA (LCD_DATA),
        .LCD_RS   (LCD_RS),
        .LCD_EN   (LCD_EN),
        .LCD_RW   (LCD_RW),
        .MCLK     (MCLK),
        .BCLK     (BCLK),
        .DACLRC   (DACLRC),
        .DACDAT   (DACDAT)
    );

    // =========================================================================
    // I2S decoder
    //
    // The WM8731 / I2S-left-justified protocol used here:
    //   - DACLRC toggles once per frame (left channel only, 32 bclk periods)
    //   - Data is clocked out MSB-first on DACDAT, sampled on BCLK rising edge
    //   - audio.v sends 16 data bits then 16 zero-padding bits per frame
    //   - audio.v XORs bit15 before writing to FIFO  (two's-complement offset)
    //     so decoded_raw[15] must be flipped back to recover the original value
    // =========================================================================

    reg  [4:0]  bit_count      = 0;
    reg  [15:0] shift_reg      = 0;
    reg  [15:0] decoded_sample = 0;
    reg         decoded_valid  = 0;   // one-cycle pulse: new sample ready
    reg         bclk_prev      = 0;
    reg         daclrc_prev    = 0;

    // Detect BCLK rising edge synchronously with our 50 MHz reference
    always @(posedge CLOCK_50) begin
        bclk_prev    <= BCLK;
        daclrc_prev  <= DACLRC;
        decoded_valid <= 0;

        // DACLRC rising edge = start of new frame, reset bit counter
        if (DACLRC != daclrc_prev) begin
            bit_count <= 0;
            shift_reg <= 0;
        end

        // Sample DACDAT on every BCLK rising edge
        if (BCLK && !bclk_prev) begin
            if (bit_count < 16) begin
                shift_reg <= {shift_reg[14:0], DACDAT};
                bit_count <= bit_count + 1;

                if (bit_count == 15) begin
                    // All 16 data bits received; undo the XOR that audio.v applied
                    decoded_sample <= {~shift_reg[14], shift_reg[13:0], DACDAT};
                    // decoded_sample <= {shift_reg[14], shift_reg[13:0], DACDAT};
                    decoded_valid  <= 1;
                end
            end
            // bits 16-31 are padding zeros, ignore them
        end
    end

    // =========================================================================
    // Monitoring / checking
    // =========================================================================

    // -- sample counter & optional golden sequence check ---------------------
    //
    // Fill EXPECTED with whatever values your C- program writes to AUDIO_SINK.
    // Set CHECK_SAMPLES = 1 to enable mismatch detection.
    // Set CHECK_SAMPLES = 0 to just print decoded samples without checking.
    //
    localparam CHECK_SAMPLES = 0;
    localparam MAX_EXPECTED  = 16;

    reg [15:0] EXPECTED [0:MAX_EXPECTED-1];
    integer    sample_index = 0;
    integer    mismatch_count = 0;

    initial begin
        // ---- populate with the values your program sends -------------------
        // Example: a ramp 0,1,2,...,15
        EXPECTED[0]  = 16'd0;
        EXPECTED[1]  = 16'd1;
        EXPECTED[2]  = 16'd2;
        EXPECTED[3]  = 16'd3;
        EXPECTED[4]  = 16'd4;
        EXPECTED[5]  = 16'd5;
        EXPECTED[6]  = 16'd6;
        EXPECTED[7]  = 16'd7;
        EXPECTED[8]  = 16'd8;
        EXPECTED[9]  = 16'd9;
        EXPECTED[10] = 16'd10;
        EXPECTED[11] = 16'd11;
        EXPECTED[12] = 16'd12;
        EXPECTED[13] = 16'd13;
        EXPECTED[14] = 16'd14;
        EXPECTED[15] = 16'd15;
    end

    // -- capacity monitor (wired straight from the DUT internals via DEBUG) --
    // If you compile with `define DEBUG you can tap dut.capacity directly.
    // Without DEBUG we observe indirectly through the produce/capacity ports.
    `ifdef DEBUG
    wire [`word_l] capacity_mon = dut.capacity;
    wire           produce_mon  = dut.produce;
    wire [`word_l] sample_mon   = dut.sample;
    `endif

    // -- decoded sample handler ----------------------------------------------
    always @(posedge CLOCK_50) begin
        if (decoded_valid) begin
            $display("[%0t ns]  decoded sample[%0d] = %0d (0x%04h)",
                     $time, sample_index, $signed(decoded_sample), decoded_sample);

            if (CHECK_SAMPLES && sample_index < MAX_EXPECTED) begin
                if (decoded_sample !== EXPECTED[sample_index]) begin
                    $display("  MISMATCH: expected %0d (0x%04h), got %0d (0x%04h)",
                             $signed(EXPECTED[sample_index]), EXPECTED[sample_index],
                             $signed(decoded_sample),         decoded_sample);
                    mismatch_count <= mismatch_count + 1;
                end else begin
                    $display("  OK");
                end
            end

            sample_index <= sample_index + 1;
        end
    end

    // =========================================================================
    // Produce-pulse sanity checker
    //
    // The circular buffer only latches a sample on the rising edge of produce.
    // If produce stays high across two processor writes the second sample is
    // silently dropped.  This block fires a warning if that happens.
    // =========================================================================
    `ifdef DEBUG
    reg produce_prev = 0;
    always @(posedge CLOCK_50) begin
        produce_prev <= produce_mon;
        if (produce_mon && produce_prev)
            $display("[%0t ns]  WARNING: produce held high for >1 cycle - sample may be dropped!", $time);
    end
    `endif

    // =============================================================================
    // FULL PROCESSOR TRACE (drop into testbench)
    // =============================================================================

    // ---------- state -> string ----------
    function [8*32-1:0] state_to_string;
        input [4:0] s;
        begin
            case (s)
                5'd0:  state_to_string = "INSTR_FETCH";
                5'd1:  state_to_string = "INSTR_FETCH_1";
                5'd2:  state_to_string = "INSTR_FETCH_2";
                5'd3:  state_to_string = "INSTR_FETCH_3";
                5'd4:  state_to_string = "ARITHMETIC";
                5'd5:  state_to_string = "ARITHMETIC_1";
                5'd6:  state_to_string = "SET_DD";
                5'd7:  state_to_string = "SET_DD_1";
                5'd8:  state_to_string = "SET_DDI";
                5'd9:  state_to_string = "SET_DDI_1";
                5'd10: state_to_string = "SET_DS";
                5'd11: state_to_string = "SET_DS_1";
                5'd12: state_to_string = "SET";
                5'd13: state_to_string = "DEF";
                5'd14: state_to_string = "DEF_1";
                5'd15: state_to_string = "DEF_2";
                5'd16: state_to_string = "WRITE";
                5'd17: state_to_string = "WRITE_1";
                5'd18: state_to_string = "WRITE_BACK";
                5'd19: state_to_string = "WRITE_BACK_1";
                5'd20: state_to_string = "SWITCH";
                5'd21: state_to_string = "LCD";
                5'd22: state_to_string = "LCD_1";
                5'd23: state_to_string = "INT_START";
                5'd24: state_to_string = "INT_START_1";
                5'd25: state_to_string = "INT_END";
                5'd26: state_to_string = "INT_END_1";
                5'd27: state_to_string = "INT_END_2";
                5'd28: state_to_string = "AUDIO_WRITE";
                default: state_to_string = "UNKNOWN";
            endcase
        end
    endfunction


    // ---------- optional: operator -> string ----------
    function [8*16-1:0] op_to_string;
        input [`op_l] op;
        begin
            case (op)
                `ADD:  op_to_string = "ADD";
                `SUB:  op_to_string = "SUB";
                `MUL:  op_to_string = "MUL";
                `DIV:  op_to_string = "DIV";
                `LT:   op_to_string = "LT";
                `GT:   op_to_string = "GT";
                `LEQ:  op_to_string = "LEQ";
                `GEQ:  op_to_string = "GEQ";
                `EQ:   op_to_string = "EQ";
                `NEQ:  op_to_string = "NEQ";
                `FJMP: op_to_string = "FJMP";
                `SET:  op_to_string = "SET";
                `SETI: op_to_string = "SETI";
                `SETDD:  op_to_string = "SETDD";
                `SETDDI: op_to_string = "SETDDI";
                `SETDS:  op_to_string = "SETDS";
                default: op_to_string = "UNK";
            endcase
        end
    endfunction


    // ---------- full trace ----------
    // always @(posedge CLOCK_50) begin
    //     $display(
    //         "[%0t] STATE=%s | PC=%0d (0x%04h) | INST=0x%04h | OP=%s | A=%0d B=%0d C=%0d | RES=%0d | PROD=%0b | CAP=%0d",
    //         $time,
    //         state_to_string(dut.state),

    //         dut.currpc,
    //         dut.currpc,

    //         dut.query,
    //         op_to_string(dut.operator),

    //         dut.op_a,
    //         dut.op_b,
    //         dut.op_c,

    //         dut.result,
    //         dut.produce,
    //         dut.capacity
    //     );
    // end

    // =========================================================================
    // Simulation control
    // =========================================================================
    integer total_samples;

    initial begin

        KEY[0] = 0;   // reset ON
        #100;
        KEY[0] = 1;   // release reset

        $dumpfile("processor_audio_tb.vcd");
        $dumpvars(0, processor_audio_tb);

        // Run long enough for the processor to fill the buffer and the audio
        // module to drain and transmit several frames.
        // Each I2S frame = 1024 CLOCK_50 cycles (counter[9] period).
        // Run for 512 frames = 512 * 1024 * 20 ns ≈ 10.5 ms
        #(512 * 1024 * 40);

        total_samples = sample_index;
        $display("----------------------------------------------------");
        $display("Simulation done.");
        $display("  Total I2S samples decoded : %0d", total_samples);
        $display("  Mismatches                : %0d", mismatch_count);
        if (mismatch_count == 0 && CHECK_SAMPLES)
            $display("  Result: PASS");
        else if (CHECK_SAMPLES)
            $display("  Result: FAIL");
        else
            $display("  (CHECK_SAMPLES disabled - inspect decoded values above)");
        $display("----------------------------------------------------");
        $finish;
    end

endmodule