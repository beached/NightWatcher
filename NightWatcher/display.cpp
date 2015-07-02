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
// Display functions.
// *************************************************************************************************

// *************************************************************************************************
// Include section

#include "display.h"
#include <cc430f6137.h>
#include <cstring>
#include <limits>
#include <algorithm>
#include <array>

namespace display {
	// *************************************************************************************************
	// @fn          lcd_init
	// @brief       Erase LCD memory. Init LCD peripheral.
	// @param      	none
	// @return      none
	// *************************************************************************************************
	void lcd_init( ) {
		// Clear entire display memory
		LCDBMEMCTL |= LCDCLRBM + LCDCLRM;

		// LCD_FREQ = ACLK/12/8 = 341.3Hz flickers in the sun
		// LCD_FREQ = ACLK/10/8 = 409.6Hz still flickers in the sun when watch is moving (might be negligible)

		// LCD_FREQ = ACLK/8/8 = 512Hz no flickering, even when watch is moving
		// Frame frequency = 512Hz/2/4 = 64Hz, LCD mux 4, LCD on
		LCDBCTL0 = (LCDDIV0 + LCDDIV1 + LCDDIV2) | (LCDPRE0 + LCDPRE1) | LCD4MUX | LCDON;

		// LCB_BLK_FREQ = ACLK/8/4096 = 1Hz
		LCDBBLKCTL = LCDBLKPRE0 | LCDBLKPRE1 | LCDBLKDIV0 | LCDBLKDIV1 | LCDBLKDIV2 | LCDBLKMOD0;

		// I/O to COM outputs
		P5SEL |= (BIT5 | BIT6 | BIT7);
		P5DIR |= (BIT5 | BIT6 | BIT7);

		// Activate LCD output
		LCDBPCTL0 = 0xFFFF;                         // Select LCD segments S0-S15
		LCDBPCTL1 = 0x00FF;                         // Select LCD segments S16-S22

#ifdef USE_LCD_CHARGE_PUMP
												// Charge pump voltage generated internally, internal bias (V2-V4) generation
		LCDBVCTL = LCDCPEN | VLCD_2_72;
#endif
	}

	// *************************************************************************************************
	// @fn          clear_display_all
	// @brief       Erase LINE1 and LINE2 segments. Clear also function-specific content.
	// @param      	none
	// @return      none
	// *************************************************************************************************
	void clear_display_all( ) {
		// Clear generic content
		clear_line( LINE1 );
		clear_line( LINE2 );
	}

	// *************************************************************************************************
	// @fn          clear_display
	// @brief       Erase LINE1 and LINE2 segments. Keep icons.
	// @param      	none
	// @return      none
	// *************************************************************************************************
	void clear_display( ) {
		clear_line( LINE1 );
		clear_line( LINE2 );
	}

	// *************************************************************************************************
	// @fn          clear_line
	// @brief       Erase segments of a given line.
	// @param      	u8 line	LINE1, LINE2
	// @return      none
	// *************************************************************************************************
	void clear_line( uint8_t line ) {
		display_chars( switch_seg( line, LCD_SEG_L1_3_0, LCD_SEG_L2_5_0 ), NULL, LcdDisplayModes::SEG_OFF );
		if( line == LINE1 ) {
			display_symbol( LCD_SEG_L1_DP1, LcdDisplayModes::SEG_OFF );
			display_symbol( LCD_SEG_L1_DP0, LcdDisplayModes::SEG_OFF );
			display_symbol( LCD_SEG_L1_COL, LcdDisplayModes::SEG_OFF );
		} else { // line == LINE2
			display_symbol( LCD_SEG_L2_DP, LcdDisplayModes::SEG_OFF );
			display_symbol( LCD_SEG_L2_COL1, LcdDisplayModes::SEG_OFF );
			display_symbol( LCD_SEG_L2_COL0, LcdDisplayModes::SEG_OFF );
		}
	}

	// *************************************************************************************************
	// @fn          write_segment
	// @brief       Write to one or multiple LCD segments
	// @param       lcdmem		Pointer to LCD byte memory
	//				bits		Segments to address
	//				bitmask		Bitmask for particular display item
	//				mode		On, off or blink segments
	// @return
	// *************************************************************************************************
	static void write_lcd_mem( uint8_t * const lcdmem, uint8_t const & bits, uint8_t const & bitmask, LcdDisplayModes const & state ) {
		switch( state ) {
		case LcdDisplayModes::SEG_ON:
			// Clear segments before writing
			*lcdmem = (uint8_t)(*lcdmem & ~bitmask);

			// Set visible segments
			*lcdmem = (uint8_t)(*lcdmem | bits);
			break;
		case LcdDisplayModes::SEG_OFF:
			// Clear segments
			*lcdmem = (uint8_t)(*lcdmem & ~bitmask);
			break;
		case LcdDisplayModes::SEG_ON_BLINK_ON:
			// Clear visible / blink segments before writing
			*lcdmem = (uint8_t)(*lcdmem & ~bitmask);
			*(lcdmem + 0x20) = (uint8_t)(*(lcdmem + 0x20) & ~bitmask);

			// Set visible / blink segments
			*lcdmem = (uint8_t)(*lcdmem | bits);
			*(lcdmem + 0x20) = (uint8_t)(*(lcdmem + 0x20) | bits);
			break;
		case LcdDisplayModes::SEG_ON_BLINK_OFF:
			// Clear visible segments before writing
			*lcdmem = (uint8_t)(*lcdmem & ~bitmask);

			// Set visible segments
			*lcdmem = (uint8_t)(*lcdmem | bits);

			// Clear blink segments
			*(lcdmem + 0x20) = (uint8_t)(*(lcdmem + 0x20) & ~bitmask);
			break;
		case LcdDisplayModes::SEG_OFF_BLINK_OFF:
			// Clear segments
			*lcdmem = (uint8_t)(*lcdmem & ~bitmask);

			// Clear blink segments
			*(lcdmem + 0x20) = (uint8_t)(*(lcdmem + 0x20) & ~bitmask);
		}
	}

	static char * itoa( uint32_t val, uint8_t const & digits, uint8_t blanks ) {
		const size_t result_str_size = 7;
		static std::array<char, result_str_size + 1> result_str;
		std::fill( std::begin( result_str ), std::begin( result_str ) + result_str_size, '0' );
		result_str[result_str_size] = '\0';

		// Return empty string if number of digits is invalid (valid range for digits: 1-7)
		if( (digits == 0) || (digits > result_str_size) ) {
			return result_str.data( );
		}
		size_t i = 1;
		for( ; i < result_str.size( ); ++i ) {
			result_str[digits - i] = (char)(val % 10);
			val /= 10;
		}

		// Remove specified number of leading '0', always keep last one
		i = 0;
		while( (result_str[i] == '0') && (i < digits - 1u) ) {
			if( blanks ) {
				// Convert only specified number of leading '0'
				result_str[i] = ' ';
				--blanks;
			}
			++i;
		}
		return result_str.data( );
	}

	// *************************************************************************************************
	// @fn          display_value1
	// @brief       Generic decimal display routine. Used exclusively by set_value function.
	// @param       u8 segments		LCD segments where value is displayed
	//				u32 value			Integer value to be displayed
	//				u8 digits			Number of digits to convert
	//				u8 blanks			Number of leadings blanks in itoa result string
	// @return      none
	// *************************************************************************************************
	void display_value1( uint8_t const & segments, uint32_t const & value, uint8_t const & digits, uint8_t const & blanks, LcdDisplayModes const & disp_mode ) {
		auto const str = itoa( value, digits, blanks );
		// Display string in blink mode
		display_chars( segments, str, disp_mode );
	}

	// *************************************************************************************************
	// @fn          display_symbol
	// @brief       Switch symbol on or off on LCD.
	// @param       u8 symbol		A valid LCD symbol (index 0..42)
	//				u8 state		SEG_ON, SEG_OFF, SEG_BLINK
	// @return      none
	// *************************************************************************************************
	void display_symbol( uint8_t symbol, LcdDisplayModes const mode ) {
		if( symbol <= LCD_SEG_L2_DP ) {
			// Get LCD memory address for symbol from table
			uint8_t * lcdmem = (uint8_t *)segments_lcdmem[symbol];

			// Get bits for symbol from table
			uint8_t bits = segments_bitmask[symbol];

			// Bitmask for symbols equals bits
			uint8_t bitmask = bits;

			// Write LCD memory
			write_lcd_mem( lcdmem, bits, bitmask, mode );
		}
	}

	uint8_t get_font( uint8_t const character ) {
		/*
		LCD Display has the following segment assignments
		A
		*****
		*   *
		F*   *B
		*   *
		* G *
		*****
		*   *
		E*   *C
		*   *
		*   *
		*****
		D
		*/
		switch( character ) {
		case 0:
		case '0': return SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F;
		case 1:
		case '1': return SEG_B | SEG_C;
		case 'z':
		case 'Z':
		case 2:
		case '2': return SEG_A | SEG_B | SEG_D | SEG_E | SEG_G;
		case 3:
		case '3': return SEG_A | SEG_B | SEG_C | SEG_D | SEG_G;
		case 4:
		case '4': return SEG_B | SEG_C | SEG_F | SEG_G;
		case 5:
		case '5': return SEG_A | SEG_C | SEG_D | SEG_F | SEG_G;
		case 6:
		case '6': return SEG_A | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G;
		case 7:
		case '7': return SEG_A | SEG_B | SEG_C;
		case 8:
		case '8': return SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G;
		case 'q':
		case 'Q':
		case 9:
		case '9': return SEG_A | SEG_B | SEG_C | SEG_D | SEG_F | SEG_G;
		case 'A': return SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G;
		case 'b':
		case 'B': return SEG_C | SEG_D | SEG_E | SEG_F | SEG_G;
		case 'c': return SEG_C | SEG_D | SEG_E | SEG_G;
		case 'C': return SEG_A | SEG_D | SEG_E | SEG_F;
		case 'd':
		case 'D': return SEG_B | SEG_C | SEG_D | SEG_E | SEG_G;
		case 'e':
		case 'E': return SEG_A | SEG_D | SEG_E | SEG_F | SEG_G;
		case 'f':
		case 'F': return SEG_A | SEG_E | SEG_F | SEG_G;
		case 'g':
		case 'G': return SEG_A | SEG_C | SEG_D | SEG_E | SEG_F;
		case 'h': return SEG_C | SEG_E | SEG_F | SEG_G;
		case 'x':
		case 'X':
		case 'H': return SEG_B | SEG_C | SEG_E | SEG_F | SEG_G;
		case 'i': return SEG_E;
		case 'I': return SEG_E | SEG_F;
		case 'j':
		case 'J': return SEG_B | SEG_C | SEG_D | SEG_F;
		case 'k':
		case 'K': return SEG_A | SEG_C | SEG_E | SEG_F | SEG_G;
		case 'l':
		case 'L': return SEG_D | SEG_E | SEG_F;
		case 'm':
		case 'M': return SEG_A | SEG_C | SEG_E;
		case 'n': return SEG_C | SEG_E | SEG_G;
		case 'N': return SEG_A | SEG_B | SEG_C | SEG_E | SEG_F;
		case 'o': return SEG_C | SEG_D | SEG_E | SEG_G;
		case 'O': return SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F;
		case 'p':
		case 'P': return SEG_A | SEG_B | SEG_E | SEG_F | SEG_G;
		case 'r':
		case 'R': return SEG_E | SEG_G;
		case 's':
		case 'S': return SEG_A | SEG_C | SEG_D | SEG_F | SEG_G;
		case 't':
		case 'T': return SEG_D | SEG_E | SEG_F | SEG_G;
		case 'u':
		case 'v': return SEG_C | SEG_D | SEG_E;
		case 'U':
		case 'V': return SEG_B | SEG_C | SEG_D | SEG_E | SEG_F;
		case 'w':
		case 'W': return SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G;
		case 'y':
		case 'Y': return SEG_B | SEG_C | SEG_D | SEG_F | SEG_G;
		case '-': return SEG_G;
		case '_': return SEG_D;
		case '(':
		case '[':
		case '<': return SEG_A | SEG_D | SEG_E | SEG_F;
		case ')':
		case ']':
		case '>': return SEG_A | SEG_B | SEG_C | SEG_D;
		case '?': return SEG_A | SEG_B | SEG_E | SEG_G;
		default:
			return 0;
		}
	}

	static uint8_t swap_nibble( uint8_t value ) {
		value = ((value << 4) & 0xF0) | ((value >> 4) & 0x0F);
		return value;
	}

	// *************************************************************************************************
	// @fn          display_char
	// @brief       Write to 7-segment characters.
	// @param       u8 segment		A valid LCD segment
	//				u8 chr			Character to display
	//				u8 mode		SEG_ON, SEG_OFF, SEG_BLINK
	// @return      none
	// *************************************************************************************************
	void display_char( uint8_t segment, unsigned char chr, LcdDisplayModes const mode ) {
		// Write to single 7-segment character
		if( (segment >= LCD_SEG_L1_3) && (segment <= LCD_SEG_L2_DP) ) {
			// Get LCD memory address for segment from table
			uint8_t * lcdmem = (uint8_t *)segments_lcdmem[segment]; // Pointer to LCD memory

																	// Get bitmask for character from table
			uint8_t bitmask = segments_bitmask[segment]; // Bitmask for character

														 // Get bits from font set
			chr = (unsigned char)get_font( chr );

			if( segment >= LCD_SEG_L2_5 ) {
				if( segment == LCD_SEG_L2_5 && chr != 0 ) {
					chr = BIT7;
				} else {
					// When addressing LINE2 7-segment characters need to swap high- and low-nibble,
					// because LCD COM/SEG assignment is mirrored against LINE1
					chr = swap_nibble( chr );
				}
			}

			// Physically write to LCD memory
			write_lcd_mem( lcdmem, chr, bitmask, mode );
		}
	}

	// *************************************************************************************************
	// @fn          display_chars
	// @brief       Write to consecutive 7-segment characters.
	// @param       u8 segments	LCD segment array
	//				u8 * str		Pointer to a string
	//				u8 mode		SEG_ON, SEG_OFF, SEG_BLINK
	// @return      none
	// *************************************************************************************************
	void display_chars( uint8_t segments, char const * const str, LcdDisplayModes const mode ) {
		uint8_t length = 0;			// Write length
		uint8_t char_start;			// Starting point for consecutive write

									//single character
		if( (segments >= LCD_SEG_L1_3) && (segments <= LCD_SEG_L2_DP) ) {
			length = 1;
			char_start = segments;
		}
		//multiple characters
		switch( segments ) {
			// LINE1
		case LCD_SEG_L1_3_0:	length = 4; char_start = LCD_SEG_L1_3; break;
		case LCD_SEG_L1_2_0:	length = 3; char_start = LCD_SEG_L1_2; break;
		case LCD_SEG_L1_1_0: 	length = 2; char_start = LCD_SEG_L1_1; break;
		case LCD_SEG_L1_3_1: 	length = 3; char_start = LCD_SEG_L1_3; break;
		case LCD_SEG_L1_3_2: 	length = 2; char_start = LCD_SEG_L1_3; break;

			// LINE2
		case LCD_SEG_L2_5_0:	length = 6; char_start = LCD_SEG_L2_5; break;
		case LCD_SEG_L2_4_0:	length = 5; char_start = LCD_SEG_L2_4; break;
		case LCD_SEG_L2_3_0:	length = 4; char_start = LCD_SEG_L2_3; break;
		case LCD_SEG_L2_2_0:	length = 3; char_start = LCD_SEG_L2_2; break;
		case LCD_SEG_L2_1_0: 	length = 2; char_start = LCD_SEG_L2_1; break;
		case LCD_SEG_L2_5_4:	length = 2; char_start = LCD_SEG_L2_5; break;
		case LCD_SEG_L2_5_2:	length = 4; char_start = LCD_SEG_L2_5; break;
		case LCD_SEG_L2_3_2:	length = 2; char_start = LCD_SEG_L2_3; break;
		case LCD_SEG_L2_4_2: 	length = 3; char_start = LCD_SEG_L2_4; break;
		case LCD_SEG_L2_4_3: 	length = 2; char_start = LCD_SEG_L2_4; break;
		}

		// Write to consecutive digits
		for( uint8_t i = 0; i < length; i++ ) {
			// Use single character routine to write display memory
			display_char( char_start + i, *(str + i), mode );
		}
	}

	// *************************************************************************************************
	// @fn          switch_seg
	// @brief       Returns index of 7-segment character. Required for display routines that can draw
	//				information on both lines.
	// @param       u8 line		LINE1, LINE2
	//				u8 index1		Index of LINE1
	//				u8 index2		Index of LINE2
	// @return      uint8
	// *************************************************************************************************
	uint8_t switch_seg( uint8_t line, uint8_t index1, uint8_t index2 ) {
		if( line == LINE1 ) {
			return index1;
		} else { // line == LINE2
			return index2;
		}
	}

	// *************************************************************************************************
	// @fn          start_blink
	// @brief       Start blinking.
	// @param       none
	// @return      none
	// *************************************************************************************************
	void start_blink( ) {
		LCDBBLKCTL |= LCDBLKMOD0;
	}

	// *************************************************************************************************
	// @fn          stop_blink
	// @brief       Stop blinking.
	// @param       none
	// @return      none
	// *************************************************************************************************
	void stop_blink( ) {
		LCDBBLKCTL &= ~LCDBLKMOD0;
	}

	// *************************************************************************************************
	// @fn          stop_blink
	// @brief       Clear blinking memory.
	// @param       none
	// @return      none
	// *************************************************************************************************
	void clear_blink_mem( ) {
		LCDBMEMCTL |= LCDCLRBM;
	}

	// *************************************************************************************************
	// @fn          set_blink_rate
	// @brief       Set blink rate register bits.
	// @param       none
	// @return      none
	// *************************************************************************************************
	void set_blink_rate( uint8_t bits ) {
		LCDBBLKCTL &= ~(BIT7 | BIT6 | BIT5);
		LCDBBLKCTL |= bits;
	}

	// *************************************************************************************************
	// @fn          display_all_off
	// @brief       Sets everything of on the display
	// @param       none
	// @return      none
	// *************************************************************************************************
	void display_all_off( ) {
		uint8_t * lcdptr = (uint8_t*)0x0A20;
		uint8_t i;

		for( i = 1; i <= 12; i++ ) {
			*lcdptr = 0x00;
			lcdptr++;
		}
	}

	// Table with memory address for each display element
	std::array<uint8_t*, 42> const segments_lcdmem = {
			LCD_SYMB_AM_MEM,
			LCD_SYMB_PM_MEM,
			LCD_SYMB_ARROW_UP_MEM,
			LCD_SYMB_ARROW_DOWN_MEM,
			LCD_SYMB_PERCENT_MEM,
			LCD_SYMB_TOTAL_MEM,
			LCD_SYMB_AVERAGE_MEM,
			LCD_SYMB_MAX_MEM,
			LCD_SYMB_BATTERY_MEM,
			LCD_UNIT_L1_FT_MEM,
			LCD_UNIT_L1_K_MEM,
			LCD_UNIT_L1_M_MEM,
			LCD_UNIT_L1_I_MEM,
			LCD_UNIT_L1_PER_S_MEM,
			LCD_UNIT_L1_PER_H_MEM,
			LCD_UNIT_L1_DEGREE_MEM,
			LCD_UNIT_L2_KCAL_MEM,
			LCD_UNIT_L2_KM_MEM,
			LCD_UNIT_L2_MI_MEM,
			LCD_ICON_HEART_MEM,
			LCD_ICON_STOPWATCH_MEM,
			LCD_ICON_RECORD_MEM,
			LCD_ICON_ALARM_MEM,
			LCD_ICON_BEEPER1_MEM,
			LCD_ICON_BEEPER2_MEM,
			LCD_ICON_BEEPER3_MEM,
			LCD_SEG_L1_3_MEM,
			LCD_SEG_L1_2_MEM,
			LCD_SEG_L1_1_MEM,
			LCD_SEG_L1_0_MEM,
			LCD_SEG_L1_COL_MEM,
			LCD_SEG_L1_DP1_MEM,
			LCD_SEG_L1_DP0_MEM,
			LCD_SEG_L2_5_MEM,
			LCD_SEG_L2_4_MEM,
			LCD_SEG_L2_3_MEM,
			LCD_SEG_L2_2_MEM,
			LCD_SEG_L2_1_MEM,
			LCD_SEG_L2_0_MEM,
			LCD_SEG_L2_COL1_MEM,
			LCD_SEG_L2_COL0_MEM,
			LCD_SEG_L2_DP_MEM,
	};

	// Table with bit mask for each display element
	std::array<uint8_t, 42> const segments_bitmask = {
		(uint8_t)LcdBitMask::LCD_SYMB_AM_MASK,
		(uint8_t)LcdBitMask::LCD_SYMB_PM_MASK,
		(uint8_t)LcdBitMask::LCD_SYMB_ARROW_UP_MASK,
		(uint8_t)LcdBitMask::LCD_SYMB_ARROW_DOWN_MASK,
		(uint8_t)LcdBitMask::LCD_SYMB_PERCENT_MASK,
		(uint8_t)LcdBitMask::LCD_SYMB_TOTAL_MASK,
		(uint8_t)LcdBitMask::LCD_SYMB_AVERAGE_MASK,
		(uint8_t)LcdBitMask::LCD_SYMB_MAX_MASK,
		(uint8_t)LcdBitMask::LCD_SYMB_BATTERY_MASK,
		(uint8_t)LcdBitMask::LCD_UNIT_L1_FT_MASK,
		(uint8_t)LcdBitMask::LCD_UNIT_L1_K_MASK,
		(uint8_t)LcdBitMask::LCD_UNIT_L1_M_MASK,
		(uint8_t)LcdBitMask::LCD_UNIT_L1_I_MASK,
		(uint8_t)LcdBitMask::LCD_UNIT_L1_PER_S_MASK,
		(uint8_t)LcdBitMask::LCD_UNIT_L1_PER_H_MASK,
		(uint8_t)LcdBitMask::LCD_UNIT_L1_DEGREE_MASK,
		(uint8_t)LcdBitMask::LCD_UNIT_L2_KCAL_MASK,
		(uint8_t)LcdBitMask::LCD_UNIT_L2_KM_MASK,
		(uint8_t)LcdBitMask::LCD_UNIT_L2_MI_MASK,
		(uint8_t)LcdBitMask::LCD_ICON_HEART_MASK,
		(uint8_t)LcdBitMask::LCD_ICON_STOPWATCH_MASK,
		(uint8_t)LcdBitMask::LCD_ICON_RECORD_MASK,
		(uint8_t)LcdBitMask::LCD_ICON_ALARM_MASK,
		(uint8_t)LcdBitMask::LCD_ICON_BEEPER1_MASK,
		(uint8_t)LcdBitMask::LCD_ICON_BEEPER2_MASK,
		(uint8_t)LcdBitMask::LCD_ICON_BEEPER3_MASK,
		(uint8_t)LcdBitMask::LCD_SEG_L1_3_MASK,
		(uint8_t)LcdBitMask::LCD_SEG_L1_2_MASK,
		(uint8_t)LcdBitMask::LCD_SEG_L1_1_MASK,
		(uint8_t)LcdBitMask::LCD_SEG_L1_0_MASK,
		(uint8_t)LcdBitMask::LCD_SEG_L1_COL_MASK,
		(uint8_t)LcdBitMask::LCD_SEG_L1_DP1_MASK,
		(uint8_t)LcdBitMask::LCD_SEG_L1_DP0_MASK,
		(uint8_t)LcdBitMask::LCD_SEG_L2_5_MASK,
		(uint8_t)LcdBitMask::LCD_SEG_L2_4_MASK,
		(uint8_t)LcdBitMask::LCD_SEG_L2_3_MASK,
		(uint8_t)LcdBitMask::LCD_SEG_L2_2_MASK,
		(uint8_t)LcdBitMask::LCD_SEG_L2_1_MASK,
		(uint8_t)LcdBitMask::LCD_SEG_L2_0_MASK,
		(uint8_t)LcdBitMask::LCD_SEG_L2_COL1_MASK,
		(uint8_t)LcdBitMask::LCD_SEG_L2_COL0_MASK,
		(uint8_t)LcdBitMask::LCD_SEG_L2_DP_MASK,
	};
}