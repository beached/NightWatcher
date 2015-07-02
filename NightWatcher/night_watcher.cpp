#include "display.h"
#include "radio.h"
#include <limits>

#include "timers.h"

namespace {
	inline void disable_watchdog( ) {
		WDTCTL = WDTPW | WDTHOLD;
	}
}

int main( ) {
	disable_watchdog( );

	display::lcd_init( );
	display::clear_display( );
	display::display_value1( display::LCD_SEG_LINE1_START, 1234, 4, 0, display::LcdDisplayModes::SEG_ON );
	display::display_value1( display::LCD_SEG_LINE2_START, 56789, 5, 0, display::LcdDisplayModes::SEG_ON );

	Radio::receive_loop( );

	while( true ) {
		__no_operation( );
	}
}