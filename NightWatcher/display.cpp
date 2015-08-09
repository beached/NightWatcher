#include "display.h"
#include <cc430f6137.h>
#include <string.h>
#include <limits.h>
#include "buffer.h"
#include "array.h"

namespace display {
	// *************************************************************************************************
	// @fn          lcd_init
	// @brief       Erase LCD memory. Init LCD peripheral.
	// @param      	none
	// @return      none
	// *************************************************************************************************
	void lcd_init( ) {
		// Clear entire display memory
		LCDBMEMCTL |= LCDCLRBM + LCDCLRM; // LCD_FREQ = ACLK/12/8 = 341.3Hz flickers in the sun
		// LCD_FREQ = ACLK/10/8 = 409.6Hz still flickers in the sun when watch is moving (might be negligible)

		// LCD_FREQ = ACLK/8/8 = 512Hz no flickering, even when watch is moving
		// Frame frequency = 512Hz/2/4 = 64Hz, LCD mux 4, LCD on
		LCDBCTL0 = (LCDDIV0 + LCDDIV1 + LCDDIV2) | (LCDPRE0 + LCDPRE1) | LCD4MUX | LCDON; // LCB_BLK_FREQ = ACLK/8/4096 = 1Hz
		LCDBBLKCTL = LCDBLKPRE0 | LCDBLKPRE1 | LCDBLKDIV0 | LCDBLKDIV1 | LCDBLKDIV2 | LCDBLKMOD0; // I/O to COM outputs
		P5SEL |= (BIT5 | BIT6 | BIT7);
		P5DIR |= (BIT5 | BIT6 | BIT7); // Activate LCD output
		LCDBPCTL0 = 0xFFFF; // Select LCD segments S0-S15
		LCDBPCTL1 = 0x00FF; // Select LCD segments S16-S22

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
	// @param      	uint8_t line	LINE1, LINE2
	// @return      none
	// *************************************************************************************************
	void clear_line( uint8_t line ) {
		using namespace display::LcdDisplayModes;
		display_chars( switch_seg( line, LCD_SEG_L1_3_0, LCD_SEG_L2_5_0 ), NULL, SEG_OFF );
		if( line == LINE1 ) {
			display_symbol( LCD_SEG_L1_DP1, SEG_OFF );
			display_symbol( LCD_SEG_L1_DP0, SEG_OFF );
			display_symbol( LCD_SEG_L1_COL, SEG_OFF );
		} else { // line == LINE2
			display_symbol( LCD_SEG_L2_DP, SEG_OFF );
			display_symbol( LCD_SEG_L2_COL1, SEG_OFF );
			display_symbol( LCD_SEG_L2_COL0, SEG_OFF );
		}
	}

	namespace {
		// *************************************************************************************************
		// @fn          write_segment
		// @brief       Write to one or multiple LCD segments
		// @param       lcdmem		Pointer to LCD byte memory
		//				bits		Segments to address
		//				bitmask		Bitmask for particular display item
		//				mode		On, off or blink segments
		// @return
		// *************************************************************************************************
		void write_lcd_mem( uint8_t * const lcdmem, uint8_t const & bits, uint8_t const & bitmask, LcdDisplayModes::mode state ) {
			switch( state ) {
			case LcdDisplayModes::SEG_ON:
				// Clear segments before writing
				*lcdmem = (uint8_t)(*lcdmem & ~bitmask); // Set visible segments
				*lcdmem = (uint8_t)(*lcdmem | bits);
				break;
			case LcdDisplayModes::SEG_OFF:
				// Clear segments
				*lcdmem = (uint8_t)(*lcdmem & ~bitmask);
				break;
			case LcdDisplayModes::SEG_ON_BLINK_ON:
				// Clear visible / blink segments before writing
				*lcdmem = (uint8_t)(*lcdmem & ~bitmask);
				*(lcdmem + 0x20) = (uint8_t)(*(lcdmem + 0x20) & ~bitmask); // Set visible / blink segments
				*lcdmem = (uint8_t)(*lcdmem | bits);
				*(lcdmem + 0x20) = (uint8_t)(*(lcdmem + 0x20) | bits);
				break;
			case LcdDisplayModes::SEG_ON_BLINK_OFF:
				// Clear visible segments before writing
				*lcdmem = (uint8_t)(*lcdmem & ~bitmask); // Set visible segments
				*lcdmem = (uint8_t)(*lcdmem | bits); // Clear blink segments
				*(lcdmem + 0x20) = (uint8_t)(*(lcdmem + 0x20) & ~bitmask);
				break;
			case LcdDisplayModes::SEG_OFF_BLINK_OFF:
				// Clear segments
				*lcdmem = (uint8_t)(*lcdmem & ~bitmask); // Clear blink segments
				*(lcdmem + 0x20) = (uint8_t)(*(lcdmem + 0x20) & ~bitmask);
			}
		}

		uint8_t * itoa( uint32_t val, uint8_t const & digits, uint8_t blanks ) {
			const size_t result_str_size = 7;

			static daw::Buffer<uint8_t, result_str_size + 1> result_str;
			result_str.fill_values( '0' );
			result_str[result_str_size] = '\0'; // Return empty string if number of digits is invalid (valid range for digits: 1-7)
			if( (digits == 0) || (digits > result_str_size) ) {
				return result_str.data( );
			}
			size_t i = 1;
			for( ; i < result_str.size( ); ++i ) {
				result_str[digits - i] += (uint8_t)(val % 10);
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

		static uint8_t swap_nibble( uint8_t value ) {
			value = ((value << 4) & 0xF0) | ((value >> 4) & 0x0F);
			return value;
		}
	}	// namespace anonymous
	// *************************************************************************************************
	// @fn          display_value
	// @brief       Generic decimal display routine. Used exclusively by set_value function.
	// @param       uint8_t segments		LCD segments where value is displayed
	//				u32 value			Integer value to be displayed
	//				uint8_t digits			Number of digits to convert
	//				uint8_t blanks			Number of leadings blanks in itoa result string
	// @return      none
	// *************************************************************************************************
	void display_value( uint8_t const & segments, uint32_t const & value, uint8_t const & digits, uint8_t const & blanks, display::LcdDisplayModes::mode const & disp_mode ) {
		char const * str = (char const *)itoa( value, digits, blanks ); // Display string in blink mode
		display_chars( segments, str, disp_mode );
	}

	// *************************************************************************************************
	// @fn          display_symbol
	// @brief       Switch symbol on or off on LCD.
	// @param       uint8_t symbol		A valid LCD symbol (index 0..42)
	//				uint8_t state		SEG_ON, SEG_OFF, SEG_BLINK
	// @return      none
	// *************************************************************************************************
	void display_symbol( uint8_t symbol, display::LcdDisplayModes::mode const & mode ) {
		if( symbol <= LCD_SEG_L2_DP ) {
			// Get LCD memory address for symbol from table
			uint8_t * lcdmem = (uint8_t *)segments_lcdmem[symbol]; // Get bits for symbol from table
			uint8_t bits = segments_bitmask[symbol]; // Bitmask for symbols equals bits
			uint8_t bitmask = bits; // Write LCD memory
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
		case 'c': return SEG_D | SEG_E | SEG_G;
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

	// *************************************************************************************************
	// @fn          display_char
	// @brief       Write to 7-segment characters.
	// @param       uint8_t segment		A valid LCD segment
	//				uint8_t chr			Character to display
	//				uint8_t mode		SEG_ON, SEG_OFF, SEG_BLINK
	// @return      none
	// *************************************************************************************************
	void display_char( uint8_t const & segment, uint8_t chr, display::LcdDisplayModes::mode const & mode ) {
		// Write to single 7-segment character
		if( (segment >= LCD_SEG_L1_3) && (segment <= LCD_SEG_L2_DP) ) {
			// Get LCD memory address for segment from table
			uint8_t * lcdmem = (uint8_t *)segments_lcdmem[segment]; // Pointer to LCD memory

																	// Get bitmask for character from table
			uint8_t bitmask = segments_bitmask[segment]; // Bitmask for character

														 // Get bits from font set
			chr = get_font( chr );

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
	namespace {
		struct LCD_SEG_INFO {
			uint8_t length;
			uint8_t char_start;
			LCD_SEG_INFO( ): length( 0 ), char_start( 0 ) { }
			LCD_SEG_INFO( uint8_t const & l, uint8_t const & c ) : length( l ), char_start( c ) { }
		};

		LCD_SEG_INFO find_seg_info( uint8_t const & segments ) {
			if( (segments >= LCD_SEG_L1_3) && (segments <= LCD_SEG_L2_DP) ) {
				return LCD_SEG_INFO( 1, segments );
			}
			//multiple characters
			switch( segments ) {
				// LINE1
			case LCD_SEG_L1_3_0:	return LCD_SEG_INFO( 4, LCD_SEG_L1_3 );
			case LCD_SEG_L1_2_0:	return LCD_SEG_INFO( 3, LCD_SEG_L1_2 );
			case LCD_SEG_L1_1_0: 	return LCD_SEG_INFO( 2, LCD_SEG_L1_1 );
			case LCD_SEG_L1_3_1: 	return LCD_SEG_INFO( 3, LCD_SEG_L1_3 );
			case LCD_SEG_L1_3_2: 	return LCD_SEG_INFO( 2, LCD_SEG_L1_3 ); // LINE2
			case LCD_SEG_L2_5_0: return LCD_SEG_INFO( 6, LCD_SEG_L2_5 );
			case LCD_SEG_L2_4_0: return LCD_SEG_INFO( 5, LCD_SEG_L2_4 );
			case LCD_SEG_L2_3_0: return LCD_SEG_INFO( 4, LCD_SEG_L2_3 );
			case LCD_SEG_L2_2_0: return LCD_SEG_INFO( 3, LCD_SEG_L2_2 );
			case LCD_SEG_L2_1_0: return LCD_SEG_INFO( 2, LCD_SEG_L2_1 );
			case LCD_SEG_L2_5_4: return LCD_SEG_INFO( 2, LCD_SEG_L2_5 );
			case LCD_SEG_L2_5_2: return LCD_SEG_INFO( 4, LCD_SEG_L2_5 );
			case LCD_SEG_L2_3_2: return LCD_SEG_INFO( 2, LCD_SEG_L2_3 );
			case LCD_SEG_L2_4_2: return LCD_SEG_INFO( 3, LCD_SEG_L2_4 );
			case LCD_SEG_L2_4_3: return LCD_SEG_INFO( 2, LCD_SEG_L2_4 );
			default:
				return LCD_SEG_INFO( 0, 0 );
			}
		}
	}
	// *************************************************************************************************
	// @fn          display_chars
	// @brief       Write to consecutive 7-segment characters.
	// @param       uint8_t segments	LCD segment array
	//				uint8_t * str		Pointer to a string
	//				uint8_t mode		SEG_ON, SEG_OFF, SEG_BLINK
	// @return      none
	// *************************************************************************************************
	void display_chars( uint8_t const & segments, char const * str, display::LcdDisplayModes::mode const & mode ) {
		// Since segments is limited and static, one could turn this into a template with that as
		// the parameter and make seg_info static
		LCD_SEG_INFO const seg_info = find_seg_info( segments ); // Write to consecutive digits
		for( uint8_t i = 0; i < seg_info.length && *str; i++ ) {
			// Use single character routine to write display memory
			display_char( seg_info.char_start + i, *(str++), mode );
		}
	}

	void display_hex_chars( uint8_t const & segments, uint8_t const * const str, display::LcdDisplayModes::mode const & mode ) {
		uint8_t const hex_nibble[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
		LCD_SEG_INFO const seg_info = find_seg_info( segments );
		for( uint8_t i = 0; i < seg_info.length && *str; i++ ) {
			uint8_t const & curr_byte = *str; // Use single character routine to write display memory
			display_char( seg_info.char_start + i, hex_nibble[(curr_byte & 0xF0) >> 4u], mode );
			if( ++i < seg_info.length ) {
				display_char( seg_info.char_start + i, hex_nibble[curr_byte & 0x0F], mode );
			}
		}
	}

	// *************************************************************************************************
	// @fn          switch_seg
	// @brief       Returns index of 7-segment character. Required for display routines that can draw
	//				information on both lines.
	// @param       uint8_t line		LINE1, LINE2
	//				uint8_t index1		Index of LINE1
	//				uint8_t index2		Index of LINE2
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
	uint8_t* const segments_lcdmem[42] = {
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
	}; // Table with bit mask for each display element
	uint8_t const segments_bitmask[42] = {
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