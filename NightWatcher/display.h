#pragma once
// *************************************************************************************************
//
//	Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
//
//
//	  Redistribution and use in source and binary forms, with or without
//	  modification, are permitted provided that the following conditions
//	  are met:
//
//	    Redistributions of source code must retain the above copyright
//	    notice, this list of conditions and the following disclaimer.
//
//	    Redistributions in binary form must reproduce the above copyright
//	    notice, this list of conditions and the following disclaimer in the
//	    documentation and/or other materials provided with the
//	    distribution.
//
//	    Neither the name of Texas Instruments Incorporated nor the names of
//	    its contributors may be used to endorse or promote products derived
//	    from this software without specific prior written permission.
//
//	  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//	  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//	  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//	  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//	  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//	  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//	  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//	  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//	  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//	  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//	  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// *************************************************************************************************

#include <stdint.h>
#include <cc430f6137.h>
#include <array>

namespace display {
	// Constants defined in library
	extern std::array<uint8_t*, 42> const segments_lcdmem;
	extern std::array<uint8_t, 42> const segments_bitmask;

	// *************************************************************************************************
	// Defines section

	// Display function modes
	uint8_t const DISPLAY_LINE_UPDATE_FULL = BIT0;
	uint8_t const DISPLAY_LINE_UPDATE_PARTIAL = BIT1;
	uint8_t const DISPLAY_LINE_CLEAR = BIT2;

	// Definitions for line view style
	uint8_t const DISPLAY_DEFAULT_VIEW = 0;
	uint8_t const DISPLAY_ALTERNATIVE_VIEW = 1;
	uint8_t const DISPLAY_ALTERNATIVE2_VIEW = 2;

	// Definitions for line access
	uint8_t const LINE1 = 1;
	uint8_t const LINE2 = 2;

	enum class LcdDisplayModes: int16_t {
		SEG_OFF = 0,
		SEG_ON = 1,
		SEG_ON_BLINK_ON = 2,
		SEG_ON_BLINK_OFF = 3,
		SEG_OFF_BLINK_OFF = 4
	};

	enum LcdSegments {
		SEG_A = BIT4,
		SEG_B = BIT5,
		SEG_C = BIT6,
		SEG_D = BIT7,
		SEG_E = BIT2,
		SEG_F = BIT0,
		SEG_G = BIT1
	};

	// ------------------------------------------
	// LCD symbols for easier access
	//
	// xxx_SEG_xxx 		= Seven-segment character (sequence 5-4-3-2-1-0)
	// xxx_SYMB_xxx 	= Display symbol, e.g. "AM" for ante meridiem
	// xxx_UNIT_xxx 	= Display unit, e.g. "km/h" for kilometers per hour
	// xxx_ICON_xxx 	= Display icon, e.g. heart to indicate reception of heart rate data
	// xxx_L1_xxx 		= Item is part of Line1 information
	// xxx_L2_xxx 		= Item is part of Line2 information

	// Symbols for Line1
	uint8_t const LCD_SYMB_AM = 0;
	uint8_t const LCD_SYMB_PM = 1;
	uint8_t const LCD_SYMB_ARROW_UP = 2;
	uint8_t const LCD_SYMB_ARROW_DOWN = 3;
	uint8_t const LCD_SYMB_PERCENT = 4;

	// Symbols for Line2
	uint8_t const LCD_SYMB_TOTAL = 5;
	uint8_t const LCD_SYMB_AVERAGE = 6;
	uint8_t const LCD_SYMB_MAX = 7;
	uint8_t const LCD_SYMB_BATTERY = 8;

	// Units for Line1
	uint8_t const LCD_UNIT_L1_FT = 9;
	uint8_t const LCD_UNIT_L1_K = 10;
	uint8_t const LCD_UNIT_L1_M = 11;
	uint8_t const LCD_UNIT_L1_I = 12;
	uint8_t const LCD_UNIT_L1_PER_S = 13;
	uint8_t const LCD_UNIT_L1_PER_H = 14;
	uint8_t const LCD_UNIT_L1_DEGREE = 15;

	// Units for Line2
	uint8_t const LCD_UNIT_L2_KCAL = 16;
	uint8_t const LCD_UNIT_L2_KM = 17;
	uint8_t const LCD_UNIT_L2_MI = 18;

	// Icons
	uint8_t const LCD_ICON_HEART = 19;
	uint8_t const LCD_ICON_STOPWATCH = 20;
	uint8_t const LCD_ICON_RECORD = 21;
	uint8_t const LCD_ICON_ALARM = 22;
	uint8_t const LCD_ICON_BEEPER1 = 23;
	uint8_t const LCD_ICON_BEEPER2 = 24;
	uint8_t const LCD_ICON_BEEPER3 = 25;

	// Line1 7-segments
	uint8_t const LCD_SEG_L1_3 = 26;
	uint8_t const LCD_SEG_L1_2 = 27;
	uint8_t const LCD_SEG_L1_1 = 28;
	uint8_t const LCD_SEG_L1_0 = 29;
	uint8_t const LCD_SEG_L1_COL = 30;
	uint8_t const LCD_SEG_L1_DP1 = 31;
	uint8_t const LCD_SEG_L1_DP0 = 32;

	// Line2 7-segments
	uint8_t const LCD_SEG_L2_5 = 33;
	uint8_t const LCD_SEG_L2_4 = 34;
	uint8_t const LCD_SEG_L2_3 = 35;
	uint8_t const LCD_SEG_L2_2 = 36;
	uint8_t const LCD_SEG_L2_1 = 37;
	uint8_t const LCD_SEG_L2_0 = 38;
	uint8_t const LCD_SEG_L2_COL1 = 39;
	uint8_t const LCD_SEG_L2_COL0 = 40;
	uint8_t const LCD_SEG_L2_DP = 41;

	// Line1 7-segment arrays
	uint8_t const LCD_SEG_L1_3_0 = 70;
	uint8_t const LCD_SEG_LINE1_START = 70;
	uint8_t const LCD_SEG_L1_2_0 = 71;
	uint8_t const LCD_SEG_L1_1_0 = 72;
	uint8_t const LCD_SEG_L1_3_1 = 73;
	uint8_t const LCD_SEG_L1_3_2 = 74;

	// Line2 7-segment arrays
	uint8_t const LCD_SEG_L2_5_0 = 90;
	uint8_t const LCD_SEG_L2_4_0 = 91;
	uint8_t const LCD_SEG_LINE2_START = 91;
	uint8_t const LCD_SEG_L2_3_0 = 92;
	uint8_t const LCD_SEG_L2_2_0 = 93;
	uint8_t const LCD_SEG_L2_1_0 = 94;
	uint8_t const LCD_SEG_L2_5_2 = 95;
	uint8_t const LCD_SEG_L2_3_2 = 96;
	uint8_t const LCD_SEG_L2_5_4 = 97;
	uint8_t const LCD_SEG_L2_4_2 = 98;
	uint8_t const LCD_SEG_L2_4_3 = 99;

	// LCD controller memory map
	uint8_t * const LCD_MEM_1 = (uint8_t*)0x0A20;
	uint8_t * const LCD_MEM_2 = (uint8_t*)0x0A21;
	uint8_t * const LCD_MEM_3 = (uint8_t*)0x0A22;
	uint8_t * const LCD_MEM_4 = (uint8_t*)0x0A23;
	uint8_t * const LCD_MEM_5 = (uint8_t*)0x0A24;
	uint8_t * const LCD_MEM_6 = (uint8_t*)0x0A25;
	uint8_t * const LCD_MEM_7 = (uint8_t*)0x0A26;
	uint8_t * const LCD_MEM_8 = (uint8_t*)0x0A27;
	uint8_t * const LCD_MEM_9 = (uint8_t*)0x0A28;
	uint8_t * const LCD_MEM_10 = (uint8_t*)0x0A29;
	uint8_t * const LCD_MEM_11 = (uint8_t*)0x0A2A;
	uint8_t * const LCD_MEM_12 = (uint8_t*)0x0A2B;

	// Memory assignment
	uint8_t * const LCD_SEG_L1_0_MEM = LCD_MEM_6;
	uint8_t * const LCD_SEG_L1_1_MEM = LCD_MEM_4;
	uint8_t * const LCD_SEG_L1_2_MEM = LCD_MEM_3;
	uint8_t * const LCD_SEG_L1_3_MEM = LCD_MEM_2;
	uint8_t * const LCD_SEG_L1_COL_MEM = LCD_MEM_1;
	uint8_t * const LCD_SEG_L1_DP1_MEM = LCD_MEM_1;
	uint8_t * const LCD_SEG_L1_DP0_MEM = LCD_MEM_5;
	uint8_t * const LCD_SEG_L2_0_MEM = LCD_MEM_8;
	uint8_t * const LCD_SEG_L2_1_MEM = LCD_MEM_9;
	uint8_t * const LCD_SEG_L2_2_MEM = LCD_MEM_10;
	uint8_t * const LCD_SEG_L2_3_MEM = LCD_MEM_11;
	uint8_t * const LCD_SEG_L2_4_MEM = LCD_MEM_12;
	uint8_t * const LCD_SEG_L2_5_MEM = LCD_MEM_12;
	uint8_t * const LCD_SEG_L2_COL1_MEM = LCD_MEM_1;
	uint8_t * const LCD_SEG_L2_COL0_MEM = LCD_MEM_5;
	uint8_t * const LCD_SEG_L2_DP_MEM = LCD_MEM_9;
	uint8_t * const LCD_SYMB_AM_MEM = LCD_MEM_1;
	uint8_t * const LCD_SYMB_PM_MEM = LCD_MEM_1;
	uint8_t * const LCD_SYMB_ARROW_UP_MEM = LCD_MEM_1;
	uint8_t * const LCD_SYMB_ARROW_DOWN_MEM = LCD_MEM_1;
	uint8_t * const LCD_SYMB_PERCENT_MEM = LCD_MEM_5;
	uint8_t * const LCD_SYMB_TOTAL_MEM = LCD_MEM_11;
	uint8_t * const LCD_SYMB_AVERAGE_MEM = LCD_MEM_10;
	uint8_t * const LCD_SYMB_MAX_MEM = LCD_MEM_8;
	uint8_t * const LCD_SYMB_BATTERY_MEM = LCD_MEM_7;
	uint8_t * const LCD_UNIT_L1_FT_MEM = LCD_MEM_5;
	uint8_t * const LCD_UNIT_L1_K_MEM = LCD_MEM_5;
	uint8_t * const LCD_UNIT_L1_M_MEM = LCD_MEM_7;
	uint8_t * const LCD_UNIT_L1_I_MEM = LCD_MEM_7;
	uint8_t * const LCD_UNIT_L1_PER_S_MEM = LCD_MEM_5;
	uint8_t * const LCD_UNIT_L1_PER_H_MEM = LCD_MEM_7;
	uint8_t * const LCD_UNIT_L1_DEGREE_MEM = LCD_MEM_5;
	uint8_t * const LCD_UNIT_L2_KCAL_MEM = LCD_MEM_7;
	uint8_t * const LCD_UNIT_L2_KM_MEM = LCD_MEM_7;
	uint8_t * const LCD_UNIT_L2_MI_MEM = LCD_MEM_7;
	uint8_t * const LCD_ICON_HEART_MEM = LCD_MEM_2;
	uint8_t * const LCD_ICON_STOPWATCH_MEM = LCD_MEM_3;
	uint8_t * const LCD_ICON_RECORD_MEM = LCD_MEM_1;
	uint8_t * const LCD_ICON_ALARM_MEM = LCD_MEM_4;
	uint8_t * const LCD_ICON_BEEPER1_MEM = LCD_MEM_5;
	uint8_t * const LCD_ICON_BEEPER2_MEM = LCD_MEM_6;
	uint8_t * const LCD_ICON_BEEPER3_MEM = LCD_MEM_7;

	enum class LcdBitMask: uint16_t {
		// Bit masks for write access
		LCD_SEG_L1_0_MASK = BIT2 + BIT1 + BIT0 + BIT7 + BIT6 + BIT5 + BIT4,
		LCD_SEG_L1_1_MASK = BIT2 + BIT1 + BIT0 + BIT7 + BIT6 + BIT5 + BIT4,
		LCD_SEG_L1_2_MASK = BIT2 + BIT1 + BIT0 + BIT7 + BIT6 + BIT5 + BIT4,
		LCD_SEG_L1_3_MASK = BIT2 + BIT1 + BIT0 + BIT7 + BIT6 + BIT5 + BIT4,
		LCD_SEG_L1_COL_MASK = BIT5,
		LCD_SEG_L1_DP1_MASK = BIT6,
		LCD_SEG_L1_DP0_MASK = BIT2,
		LCD_SEG_L2_0_MASK = BIT3 + BIT2 + BIT1 + BIT0 + BIT6 + BIT5 + BIT4,
		LCD_SEG_L2_1_MASK = BIT3 + BIT2 + BIT1 + BIT0 + BIT6 + BIT5 + BIT4,
		LCD_SEG_L2_2_MASK = BIT3 + BIT2 + BIT1 + BIT0 + BIT6 + BIT5 + BIT4,
		LCD_SEG_L2_3_MASK = BIT3 + BIT2 + BIT1 + BIT0 + BIT6 + BIT5 + BIT4,
		LCD_SEG_L2_4_MASK = BIT3 + BIT2 + BIT1 + BIT0 + BIT6 + BIT5 + BIT4,
		LCD_SEG_L2_5_MASK = BIT7,
		LCD_SEG_L2_COL1_MASK = BIT4,
		LCD_SEG_L2_COL0_MASK = BIT0,
		LCD_SEG_L2_DP_MASK = BIT7,
		LCD_SYMB_AM_MASK = BIT1 + BIT0,
		LCD_SYMB_PM_MASK = BIT0,
		LCD_SYMB_ARROW_UP_MASK = BIT2,
		LCD_SYMB_ARROW_DOWN_MASK = BIT3,
		LCD_SYMB_PERCENT_MASK = BIT4,
		LCD_SYMB_TOTAL_MASK = BIT7,
		LCD_SYMB_AVERAGE_MASK = BIT7,
		LCD_SYMB_MAX_MASK = BIT7,
		LCD_SYMB_BATTERY_MASK = BIT7,
		LCD_UNIT_L1_FT_MASK = BIT5,
		LCD_UNIT_L1_K_MASK = BIT6,
		LCD_UNIT_L1_M_MASK = BIT1,
		LCD_UNIT_L1_I_MASK = BIT0,
		LCD_UNIT_L1_PER_S_MASK = BIT7,
		LCD_UNIT_L1_PER_H_MASK = BIT2,
		LCD_UNIT_L1_DEGREE_MASK = BIT1,
		LCD_UNIT_L2_KCAL_MASK = BIT4,
		LCD_UNIT_L2_KM_MASK = BIT5,
		LCD_UNIT_L2_MI_MASK = BIT6,
		LCD_ICON_HEART_MASK = BIT3,
		LCD_ICON_STOPWATCH_MASK = BIT3,
		LCD_ICON_RECORD_MASK = BIT7,
		LCD_ICON_ALARM_MASK = BIT3,
		LCD_ICON_BEEPER1_MASK = BIT3,
		LCD_ICON_BEEPER2_MASK = BIT3,
		LCD_ICON_BEEPER3_MASK = BIT3
	};

	// *************************************************************************************************
	// API section

	// Display init / clear
	extern void lcd_init( );
	extern void clear_display( );
	extern void clear_display_all( );
	extern void clear_line( uint8_t line );

	// Blinking function
	extern void start_blink( );
	extern void stop_blink( );
	extern void clear_blink_mem( );
	extern void set_blink_rate( uint8_t bits );

	// Character / symbol draw functions
	extern void display_char( uint8_t const & segment, unsigned char chr, LcdDisplayModes const mode );
	extern void display_chars( uint8_t const & segments, char const * const str, LcdDisplayModes const mode );
	extern void display_hex_chars( uint8_t const & segments, char const * const str, LcdDisplayModes const mode );
	extern void display_symbol( uint8_t symbol, LcdDisplayModes const mode );

	// Set_value display functions
	extern void display_value( uint8_t const & segments, uint32_t const & value, uint8_t const & digits, uint8_t const & blanks, LcdDisplayModes const & disp_mode );

	// Segment index helper function
	extern uint8_t switch_seg( uint8_t line, uint8_t index1, uint8_t index2 );

	void display_all_off( );
}
