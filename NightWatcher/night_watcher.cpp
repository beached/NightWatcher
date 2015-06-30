#include "display.h"
#include "radio.h"

namespace {
	inline void disable_watchdog( ) {
		WDTCTL = WDTPW | WDTHOLD;
	}
}

int main( ) {
	disable_watchdog( );

	display::lcd_init( );
	// auto msg = "HeLLo";
	//display_chars( display::LCD_SEG_L1_3_0, "0124", display::LcdDisplayModes::SEG_ON );
	display::display_value1( display::LCD_SEG_L1_3_0, 1234, 4, 0, display::LcdDisplayModes::SEG_ON );
	display::display_value1( display::LCD_SEG_L2_4_0, 56789, 5, 0, display::LcdDisplayModes::SEG_ON );
	//display_chars( display::LCD_SEG_L2_4_0, msg, display::LcdDisplayModes::SEG_ON );

	Radio radio;

	__no_operation( );
}