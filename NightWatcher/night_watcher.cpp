#include "display.h"
#include "radio.h"
#include <limits>

#include "timers.h"

namespace {
	inline void disable_watchdog( ) {
		WDTCTL = WDTPW | WDTHOLD;
	}
}

static uint8_t s_timer_counter = 0;

int main( ) {
	disable_watchdog( );

	display::lcd_init( );
	display::clear_display( );
	// auto msg = "HeLLo";
	//display_chars( display::LCD_SEG_L1_3_0, "0124", display::LcdDisplayModes::SEG_ON );
// 	display::display_value1( display::LCD_SEG_LINE1_START, 1234, 4, 0, display::LcdDisplayModes::SEG_ON );
// 	display::display_value1( display::LCD_SEG_LINE2_START, 56789, 5, 0, display::LcdDisplayModes::SEG_ON );
	//display_chars( display::LCD_SEG_L2_4_0, msg, display::LcdDisplayModes::SEG_ON );

	//Radio radio;

	// Test Timer

	timers::Timer0::set_callback( 0, []( ) {
		auto value = s_timer_counter++;
		//display::clear_line( display::LINE1 );
		display::display_value1( display::LCD_SEG_LINE1_START, value, 4, 4, display::LcdDisplayModes::SEG_ON_BLINK_OFF );
	} );

	// 	timers::Timer1::set_callback( 1, []( ) {
	// 		auto value = s_timer_counter;
	// 		display::clear_line( display::LINE2 );
	// 		display::display_value1( display::LCD_SEG_LINE2_START, value, 4, 4, display::LcdDisplayModes::SEG_ON_BLINK_OFF );
	// 	} );
	//
	timers::Timer0::enable( 50000 );

	__bis_SR_register( LPM0_bits + GIE );       // Enter LPM0, enable interrupts

	while( true ) {
		__no_operation( );
	}
}