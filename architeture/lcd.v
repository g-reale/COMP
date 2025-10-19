/*
 SW8 (GLOBAL RESET) resets LCD
ENTITY LCD_Display IS
-- Enter number of live Hex hardware data values to display
-- (do not count ASCII character constants)
	GENERIC(Num_Hex_Digits: Integer:= 2); 
-----------------------------------------------------------------------
-- LCD Displays 16 Characters on 2 lines
-- LCD_display string is an ASCII character string entered in hex for 
-- the two lines of the  LCD Display   (See ASCII to hex table below)
-- Edit LCD_Display_String entries above to modify display
-- Enter the ASCII character's 2 hex digit equivalent value
-- (see table below for ASCII hex values)
-- To display character assign ASCII value to LCD_display_string(x)
-- To skip a character use 8'h20" (ASCII space)
-- To dislay "live" hex values from hardware on LCD use the following: 
--   make array element for that character location 8'h0" & 4-bit field from Hex_Display_Data
--   state machine sees 8'h0" in high 4-bits & grabs the next lower 4-bits from Hex_Display_Data input
--   and performs 4-bit binary to ASCII conversion needed to print a hex digit
--   Num_Hex_Digits must be set to the count of hex data characters (ie. "00"s) in the display
--   Connect hardware bits to display to Hex_Display_Data input
-- To display less than 32 characters, terminate string with an entry of 8'hFE"
--  (fewer characters may slightly increase the LCD's data update rate)
------------------------------------------------------------------- 
--                        ASCII HEX TABLE
--  Hex						Low Hex Digit
-- Value  0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
------\----------------------------------------------------------------
--H  2 |  SP  !   "   #   $   %   &   '   (   )   *   +   ,   -   .   /
--i  3 |  0   1   2   3   4   5   6   7   8   9   :   ;   <   =   >   ?
--g  4 |  @   A   B   C   D   E   F   G   H   I   J   K   L   M   N   O
--h  5 |  P   Q   R   S   T   U   V   W   X   Y   Z   [   \   ]   ^   _
--   6 |  `   a   b   c   d   e   f   g   h   i   j   k   l   m   n   o
--   7 |  p   q   r   s   t   u   v   w   x   y   z   {   |   }   ~ DEL
-----------------------------------------------------------------------
-- Example "A" is row 4 column 1, so hex value is 8'h41"
-- *see LCD Controller's Datasheet for other graphics characters available
*/
		
module lcd(clock, ascii, ready, consume, lcd_rs, lcd_en, lcd_rw, lcd_data);

input clock, consume;
input [7:0] ascii;
output lcd_rs, lcd_en, lcd_rw;
output reg ready;
inout [7:0] lcd_data;

parameter
HOLD = 4'h0,
FUNC_SET = 4'h1,
DISPLAY_ON = 4'h2,
MODE_SET = 4'h3,
WRITE = 4'h4,
LINE2 = 4'h5,
RETURN_HOME = 4'h6,
DROP_LCD_EN = 4'h7,
RESET1 = 4'h8,
RESET2 = 4'h9,
RESET3 = 4'ha,
DISPLAY_OFF = 4'hb,
DISPLAY_CLEAR = 4'hc,
WAIT = 4'hd;

reg [3:0] state, next_command;
// Enter new ASCII hex data above for LCD Display
reg [7:0] ldc_data_value;
reg [19:0] CLK_COUNT_400HZ;
reg [4:0] CHAR_COUNT;
reg CLK_400HZ, lcd_rw_int, lcd_en, lcd_rs;

// BIDIRECTIONAL TRI STATE LCD DATA BUS
assign lcd_data = (lcd_rw_int? 8'bZZZZZZZZ: ldc_data_value);
assign lcd_rw = lcd_rw_int;

always @(posedge clock)
	if (CLK_COUNT_400HZ < 20'h0F424)
	begin
	   CLK_COUNT_400HZ <= CLK_COUNT_400HZ + 1'b1;
	end
	else
	begin
	  CLK_COUNT_400HZ <= 20'h00000;
	  CLK_400HZ <= ~CLK_400HZ;
	end

// State Machine to send commands and data to LCD DISPLAY
always @(posedge CLK_400HZ)
	case (state)
	RESET1:			
// Set Function to 8-bit transfer and 2 line display with 5x8 Font size
// see Hitachi HD44780 family data sheet for LCD command and timing details
	begin
      ready <= 0;
	  lcd_en <= 1'b1;
	  lcd_rs <= 1'b0;
	  lcd_rw_int <= 1'b0;
	  ldc_data_value <= 8'h38;
	  state <= DROP_LCD_EN;
	  next_command <= RESET2;
	  CHAR_COUNT <= 5'b00000;
	end
	RESET2:
	begin
	  lcd_en <= 1'b1;
	  lcd_rs <= 1'b0;
	  lcd_rw_int <= 1'b0;
	  ldc_data_value <= 8'h38;
	  state <= DROP_LCD_EN;
	  next_command <= RESET3;
	end
	RESET3:
	begin
	  lcd_en <= 1'b1;
	  lcd_rs <= 1'b0;
	  lcd_rw_int <= 1'b0;
	  ldc_data_value <= 8'h38;
	  state <= DROP_LCD_EN;
	  next_command <= FUNC_SET;
	end
// EXTRA STATES ABOVE ARE NEEDED FOR RELIABLE PUSHBUTTON RESET OF LCD

	FUNC_SET:
	begin
	  lcd_en <= 1'b1;
	  lcd_rs <= 1'b0;
	  lcd_rw_int <= 1'b0;
	  ldc_data_value <= 8'h38;
	  state <= DROP_LCD_EN;
	  next_command <= DISPLAY_OFF;
	end

// Turn off Display and Turn off cursor
	DISPLAY_OFF:
	begin
	  lcd_en <= 1'b1;
	  lcd_rs <= 1'b0;
	  lcd_rw_int <= 1'b0;
	  ldc_data_value <= 8'h08;
	  state <= DROP_LCD_EN;
	  next_command <= DISPLAY_CLEAR;
	end

// Clear Display and Turn off cursor
	DISPLAY_CLEAR:
	begin
	  lcd_en <= 1'b1;
	  lcd_rs <= 1'b0;
	  lcd_rw_int <= 1'b0;
	  ldc_data_value <= 8'h01;
	  state <= DROP_LCD_EN;
	  next_command <= DISPLAY_ON;
	end

// Turn on Display and Turn off cursor
	DISPLAY_ON:
	begin
	  lcd_en <= 1'b1;
	  lcd_rs <= 1'b0;
	  lcd_rw_int <= 1'b0;
	  ldc_data_value <= 8'h0C;
	  state <= DROP_LCD_EN;
	  next_command <= MODE_SET;
	end

// Set write mode to auto increment address and move cursor to the right
	MODE_SET:
	begin
	  lcd_en <= 1'b1;
	  lcd_rs <= 1'b0;
	  lcd_rw_int <= 1'b0;
	  ldc_data_value <= 8'h06;
	  state <= DROP_LCD_EN;
	  next_command <= WAIT;
	end

    WAIT:
    begin
        state <=  ~consume ? WAIT :
                  ascii == 255 ? RESET1 :
                  WRITE;
        ready <=  ~consume;
    end

// Write ASCII hex character in first LCD character location
	WRITE:
	begin
	  state <= DROP_LCD_EN;
	  lcd_en <= 1'b1;
	  lcd_rs <= 1'b1;
	  lcd_rw_int <= 1'b0;
	// ASCII character to output
	  if (ascii[7:4] != 4'h0)
		ldc_data_value <= ascii;
		// Convert 4-bit value to an ASCII hex digit
	  else if (ascii[3:0] >9)
		// ASCII A...F
		 ldc_data_value <= {4'h4,ascii[3:0]};//-4'h9};
	  else
		// ASCII 0...9
		 ldc_data_value <= {4'h3,ascii[3:0]};
	// Loop to send out 32 characters to LCD Display  (16 by 2 lines)
	  if ((CHAR_COUNT < 31) && (ascii != 8'hFE))
	     CHAR_COUNT <= CHAR_COUNT + 1'b1;
	  else
	     CHAR_COUNT <= 5'b00000; 
	// Jump to second line?
	  if (CHAR_COUNT == 15)
	    next_command <= LINE2;
	// Return to first line?
	  else if ((CHAR_COUNT == 31) || (ascii == 8'hFE))
	    next_command <= RETURN_HOME;
	  else
	    next_command <= WAIT;
	end

// Set write address to line 2 character 1
	LINE2:
	begin
	  lcd_en <= 1'b1;
	  lcd_rs <= 1'b0;
	  lcd_rw_int <= 1'b0;
	  ldc_data_value <= 8'hC0;
	  state <= DROP_LCD_EN;
	  next_command <= WAIT;
	end

// Return write address to first character postion on line 1
	RETURN_HOME:
	begin
	  lcd_en <= 1'b1;
	  lcd_rs <= 1'b0;
	  lcd_rw_int <= 1'b0;
	  ldc_data_value <= 8'h80;
	  state <= DROP_LCD_EN;
	  next_command <= WAIT;
	end

// The next three states occur at the end of each command or data transfer to the LCD
// Drop LCD E line - falling edge loads inst/data to LCD controller
	DROP_LCD_EN:
	begin
	  lcd_en <= 1'b0;
	  state <= HOLD;
	end
// Hold LCD inst/data valid after falling edge of E line				
	HOLD:
	begin
	  state <= next_command;
	end
	endcase
endmodule