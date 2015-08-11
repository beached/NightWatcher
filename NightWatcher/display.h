#pragma once

#include <stdint.h>
#include <cc430f6137.h>

// *************************************************************************************************
// Defines section
namespace daw {
	namespace display {
		namespace defines {
			// Definitions for line access
			enum LCDLineAccess: uint8_t {
				LINE1 = 1,
				LINE2 = 2
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

			enum LcdSymbols: uint8_t {
			// Symbols for Line1
				LCD_SYMB_AM = 0,
				LCD_SYMB_PM = 1,
				LCD_SYMB_ARROW_UP = 2,
				LCD_SYMB_ARROW_DOWN = 3,
				LCD_SYMB_PERCENT = 4,

				// Symbols for Line2
				LCD_SYMB_TOTAL = 5,
				LCD_SYMB_AVERAGE = 6,
				LCD_SYMB_MAX = 7,
				LCD_SYMB_BATTERY = 8,

				// Units for Line1
				LCD_UNIT_L1_FT = 9,
				LCD_UNIT_L1_K = 10,
				LCD_UNIT_L1_M = 11,
				LCD_UNIT_L1_I = 12,
				LCD_UNIT_L1_PER_S = 13,
				LCD_UNIT_L1_PER_H = 14,
				LCD_UNIT_L1_DEGREE = 15,

				// Units for Line2
				LCD_UNIT_L2_KCAL = 16,
				LCD_UNIT_L2_KM = 17,
				LCD_UNIT_L2_MI = 18,

				// Icons
				LCD_ICON_HEART = 19,
				LCD_ICON_STOPWATCH = 20,
				LCD_ICON_RECORD = 21,
				LCD_ICON_ALARM = 22,
				LCD_ICON_BEEPER1 = 23,
				LCD_ICON_BEEPER2 = 24,
				LCD_ICON_BEEPER3 = 25,

				// Line1 7-segments
				LCD_SEG_L1_3 = 26,
				LCD_SEG_L1_2 = 27,
				LCD_SEG_L1_1 = 28,
				LCD_SEG_L1_0 = 29,
				LCD_SEG_L1_COL = 30,
				LCD_SEG_L1_DP1 = 31,
				LCD_SEG_L1_DP0 = 32,

				// Line2 7-segments
				LCD_SEG_L2_5 = 33,
				LCD_SEG_L2_4 = 34,
				LCD_SEG_L2_3 = 35,
				LCD_SEG_L2_2 = 36,
				LCD_SEG_L2_1 = 37,
				LCD_SEG_L2_0 = 38,
				LCD_SEG_L2_COL1 = 39,
				LCD_SEG_L2_COL0 = 40,
				LCD_SEG_L2_DP = 41,

				// Line1 7-segment arrays
				LCD_SEG_L1_3_0 = 70,
				LCD_SEG_LINE1_START = 70,
				LCD_SEG_L1_2_0 = 71,
				LCD_SEG_L1_1_0 = 72,
				LCD_SEG_L1_3_1 = 73,
				LCD_SEG_L1_3_2 = 74,

				// Line2 7-segment arrays
				LCD_SEG_L2_5_0 = 90,
				LCD_SEG_L2_4_0 = 91,
				LCD_SEG_LINE2_START = 91,
				LCD_SEG_L2_3_0 = 92,
				LCD_SEG_L2_2_0 = 93,
				LCD_SEG_L2_1_0 = 94,
				LCD_SEG_L2_5_2 = 95,
				LCD_SEG_L2_3_2 = 96,
				LCD_SEG_L2_5_4 = 97,
				LCD_SEG_L2_4_2 = 98,
				LCD_SEG_L2_4_3 = 99
			};

			// LCD controller memory map
			namespace LCDMemoryMap {
				uint8_t * const LCD_MEM_1 = reinterpret_cast<uint8_t * const>(0x0A20);
				uint8_t * const LCD_MEM_2 = reinterpret_cast<uint8_t * const>(0x0A21);
				uint8_t * const LCD_MEM_3 = reinterpret_cast<uint8_t * const>(0x0A22);
				uint8_t * const LCD_MEM_4 = reinterpret_cast<uint8_t * const>(0x0A23);
				uint8_t * const LCD_MEM_5 = reinterpret_cast<uint8_t * const>(0x0A24);
				uint8_t * const LCD_MEM_6 = reinterpret_cast<uint8_t * const>(0x0A25);
				uint8_t * const LCD_MEM_7 = reinterpret_cast<uint8_t * const>(0x0A26);
				uint8_t * const LCD_MEM_8 = reinterpret_cast<uint8_t * const>(0x0A27);
				uint8_t * const LCD_MEM_9 = reinterpret_cast<uint8_t * const>(0x0A28);
				uint8_t * const LCD_MEM_10 = reinterpret_cast<uint8_t * const>(0x0A29);
				uint8_t * const LCD_MEM_11 = reinterpret_cast<uint8_t * const>(0x0A2A);
				uint8_t * const LCD_MEM_12 = reinterpret_cast<uint8_t * const>(0x0A2B);
			}

			namespace LCDMemoryAssignment {
				using namespace defines;
				uint8_t * const LCD_SEG_L1_0_MEM = LCDMemoryMap::LCD_MEM_6;
				uint8_t * const LCD_SEG_L1_1_MEM = LCDMemoryMap::LCD_MEM_4;
				uint8_t * const LCD_SEG_L1_2_MEM = LCDMemoryMap::LCD_MEM_3;
				uint8_t * const LCD_SEG_L1_3_MEM = LCDMemoryMap::LCD_MEM_2;
				uint8_t * const LCD_SEG_L1_COL_MEM = LCDMemoryMap::LCD_MEM_1;
				uint8_t * const LCD_SEG_L1_DP1_MEM = LCDMemoryMap::LCD_MEM_1;
				uint8_t * const LCD_SEG_L1_DP0_MEM = LCDMemoryMap::LCD_MEM_5;
				uint8_t * const LCD_SEG_L2_0_MEM = LCDMemoryMap::LCD_MEM_8;
				uint8_t * const LCD_SEG_L2_1_MEM = LCDMemoryMap::LCD_MEM_9;
				uint8_t * const LCD_SEG_L2_2_MEM = LCDMemoryMap::LCD_MEM_10;
				uint8_t * const LCD_SEG_L2_3_MEM = LCDMemoryMap::LCD_MEM_11;
				uint8_t * const LCD_SEG_L2_4_MEM = LCDMemoryMap::LCD_MEM_12;
				uint8_t * const LCD_SEG_L2_5_MEM = LCDMemoryMap::LCD_MEM_12;
				uint8_t * const LCD_SEG_L2_COL1_MEM = LCDMemoryMap::LCD_MEM_1;
				uint8_t * const LCD_SEG_L2_COL0_MEM = LCDMemoryMap::LCD_MEM_5;
				uint8_t * const LCD_SEG_L2_DP_MEM = LCDMemoryMap::LCD_MEM_9;
				uint8_t * const LCD_SYMB_AM_MEM = LCDMemoryMap::LCD_MEM_1;
				uint8_t * const LCD_SYMB_PM_MEM = LCDMemoryMap::LCD_MEM_1;
				uint8_t * const LCD_SYMB_ARROW_UP_MEM = LCDMemoryMap::LCD_MEM_1;
				uint8_t * const LCD_SYMB_ARROW_DOWN_MEM = LCDMemoryMap::LCD_MEM_1;
				uint8_t * const LCD_SYMB_PERCENT_MEM = LCDMemoryMap::LCD_MEM_5;
				uint8_t * const LCD_SYMB_TOTAL_MEM = LCDMemoryMap::LCD_MEM_11;
				uint8_t * const LCD_SYMB_AVERAGE_MEM = LCDMemoryMap::LCD_MEM_10;
				uint8_t * const LCD_SYMB_MAX_MEM = LCDMemoryMap::LCD_MEM_8;
				uint8_t * const LCD_SYMB_BATTERY_MEM = LCDMemoryMap::LCD_MEM_7;
				uint8_t * const LCD_UNIT_L1_FT_MEM = LCDMemoryMap::LCD_MEM_5;
				uint8_t * const LCD_UNIT_L1_K_MEM = LCDMemoryMap::LCD_MEM_5;
				uint8_t * const LCD_UNIT_L1_M_MEM = LCDMemoryMap::LCD_MEM_7;
				uint8_t * const LCD_UNIT_L1_I_MEM = LCDMemoryMap::LCD_MEM_7;
				uint8_t * const LCD_UNIT_L1_PER_S_MEM = LCDMemoryMap::LCD_MEM_5;
				uint8_t * const LCD_UNIT_L1_PER_H_MEM = LCDMemoryMap::LCD_MEM_7;
				uint8_t * const LCD_UNIT_L1_DEGREE_MEM = LCDMemoryMap::LCD_MEM_5;
				uint8_t * const LCD_UNIT_L2_KCAL_MEM = LCDMemoryMap::LCD_MEM_7;
				uint8_t * const LCD_UNIT_L2_KM_MEM = LCDMemoryMap::LCD_MEM_7;
				uint8_t * const LCD_UNIT_L2_MI_MEM = LCDMemoryMap::LCD_MEM_7;
				uint8_t * const LCD_ICON_HEART_MEM = LCDMemoryMap::LCD_MEM_2;
				uint8_t * const LCD_ICON_STOPWATCH_MEM = LCDMemoryMap::LCD_MEM_3;
				uint8_t * const LCD_ICON_RECORD_MEM = LCDMemoryMap::LCD_MEM_1;
				uint8_t * const LCD_ICON_ALARM_MEM = LCDMemoryMap::LCD_MEM_4;
				uint8_t * const LCD_ICON_BEEPER1_MEM = LCDMemoryMap::LCD_MEM_5;
				uint8_t * const LCD_ICON_BEEPER2_MEM = LCDMemoryMap::LCD_MEM_6;
				uint8_t * const LCD_ICON_BEEPER3_MEM = LCDMemoryMap::LCD_MEM_7;
			}
		} // namespace defines

		enum LcdDisplayModes: uint8_t {
			SEG_OFF = 0,
			SEG_ON = 1,
			SEG_ON_BLINK_ON = 2,
			SEG_ON_BLINK_OFF = 3,
			SEG_OFF_BLINK_OFF = 4
		};

		enum LcdSegment: uint16_t {
			SEG_A = BIT4,
			SEG_B = BIT5,
			SEG_C = BIT6,
			SEG_D = BIT7,
			SEG_E = BIT2,
			SEG_F = BIT0,
			SEG_G = BIT1
		};

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
		extern void clear_line( uint8_t const& line ); // Blinking function
		extern void start_blink( );
		extern void stop_blink( );
		extern void clear_blink_mem( );
		extern void set_blink_rate( uint8_t bits ); // Character / symbol draw functions
		extern void display_char( uint8_t const& segment, uint8_t chr, LcdDisplayModes const mode );
		extern void display_chars( uint8_t const& segments, char const* const str, LcdDisplayModes const mode );
		extern void display_hex_chars( uint8_t const& segments, uint8_t const* const str, LcdDisplayModes const mode );
		extern void display_symbol( defines::LcdSymbols const & symbol, LcdDisplayModes const mode ); // Set_value display functions
		extern void display_value( uint8_t const& segments, uint32_t const& value, uint8_t const& digits, uint8_t const& blanks, LcdDisplayModes const disp_mode ); // Segment index helper function
		extern uint8_t switch_seg( uint8_t const & line, uint8_t const & index1, uint8_t const & index2 );

		extern void display_all_off( );
	} // namespace display
} // namespace daw
