#include <cc430f6137.h>
#include <cstdint>
#include <cstddef>

#include "display.h"

namespace {
	void delay( ) {
		for( uint32_t counter = 0; counter < 5000; ++counter ) {
			asm( "nop" );
		}
	}

	inline void disable_watchdog( ) {
		WDTCTL = WDTPW | WDTHOLD;
	}

	void main_loop( ) {
		disable_watchdog( );

		lcd_init( );
		auto msg = "HeLLo";
		//		display_chars( LCD_SEG_L1_3_0, "0124", LcdDisplayModes::SEG_ON );
		//display_value1( LCD_SEG_L1_3_0, 1234, 4, 0, LcdDisplayModes::SEG_ON );
		display_value1( LCD_SEG_L2_4_0, 12345, 5, 0, LcdDisplayModes::SEG_ON );
		//display_chars( LCD_SEG_L2_4_0, msg, LcdDisplayModes::SEG_ON );
		__no_operation( );
	}
}

int main( ) {
	main_loop( );
}