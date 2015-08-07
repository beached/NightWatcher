#include "display.h"
#include "radio_core.h"
#include "radio_medtronic.h"
#include <limits>
#include <cc430f6137.h>
#include "registers.h"
#include <type_traits>

namespace {
	const uint8_t RX_TIMER_PERIOD = 85;

	inline void disable_watchdog( ) {
		WDTCTL = WDTPW | WDTHOLD;
	}

	void toggle_net_activity( ) {
		static uint32_t count = 0;
		if( ++count > 99999 ) {
			count = 0;
		}
		display::display_value( display::LCD_SEG_LINE2_START, count, 5, 0, display::LcdDisplayModes::SEG_ON );
		switch( ++count % 4 ) {
		case 0:
			display::display_symbol( display::LCD_ICON_BEEPER3, display::LcdDisplayModes::SEG_OFF );
			break;
		case 1:
			display::display_symbol( display::LCD_ICON_BEEPER1, display::LcdDisplayModes::SEG_ON );
			break;
		case 2:
			display::display_symbol( display::LCD_ICON_BEEPER1, display::LcdDisplayModes::SEG_OFF );
			display::display_symbol( display::LCD_ICON_BEEPER2, display::LcdDisplayModes::SEG_ON );
			break;
		case 3:
			display::display_symbol( display::LCD_ICON_BEEPER2, display::LcdDisplayModes::SEG_OFF );
			display::display_symbol( display::LCD_ICON_BEEPER3, display::LcdDisplayModes::SEG_ON );
			break;
		}
	}

	daw::radio::RadioCore<256> radio;

	void setup_hardware( ) {
		daw::radio::set_vcore( 2u );
		daw::radio::init_fll( 8500000 / 1000, 8500000 / 32768 );

		display::lcd_init( );
		display::clear_display( );
		display::display_chars( display::LCD_SEG_LINE1_START, "On", display::LcdDisplayModes::SEG_ON );
		//init_timer( );
		radio.init_radio( radio_setup_916MHz );
	}

	using void_fptr = std::add_pointer<void( )>::type;
	void_fptr current_state = nullptr;

	void state_waiting_for_interrupt( );
	void state_received_data( );
	void state_process_data( );
	void state_display_data( );

	void state_process_data( ) {
		toggle_net_activity( );
		if( radio.has_data( ) ) {
			// Do something with buffer
			current_state = state_display_data;
		} else {
			current_state = state_waiting_for_interrupt;
		}
	}

	void state_display_data( ) {
		// Display glucose or something
		current_state = state_waiting_for_interrupt;
	}

	void state_received_data( ) {
		auto data_size = radio.receive_data( );
		if( 0 < data_size ) {
			current_state = state_process_data;
		} else {
			current_state = state_waiting_for_interrupt;
		}
	}

	void state_waiting_for_interrupt( ) {
		radio.receive_on( );
		_enable_interrupts( );
		__low_power_mode_2( );
		__no_operation( );
		if( radio.data_pending( ) ) {
			current_state = state_received_data;
		}
	}
}	// namespace anonymous

int main( ) {
	// Setup
	disable_watchdog( );
	current_state = state_waiting_for_interrupt;
	setup_hardware( );

	while( true ) {
		current_state( );
	}
}

void __attribute__( (interrupt( CC1101_VECTOR )) ) radio_isr( ) {
	switch( __even_in_range( RF1AIV, 32 ) ) {	// Prioritizing Radio Core Interrupt
	case  0: break;                         // No RF core interrupt pending
	case  2: break;                         // RFIFG0
	case  4:								// RFIFG1
		RF1AIE &= ~(BIT1 | BIT9);
		daw::radio::strobe( RF_SWOR );		// Go back to sleep
		P1OUT ^= BIT0;
		break;
	case  6: break;                         // RFIFG2
	case  8: break;                         // RFIFG3
	case 10: break;                         // RFIFG4
	case 12: break;                         // RFIFG5
	case 14: break;                         // RFIFG6
	case 16: break;                         // RFIFG7
	case 18: break;                         // RFIFG8
	case 20:
		RF1AIE &= ~(BIT1 | BIT9);
		if( radio.is_receiving( ) ) {
			radio.check_for_data( );
			__no_operation( );
		} else {
			while( true ) {/* spin */ }	// trap
		}
		_low_power_mode_off_on_exit( );
		break;
	case 22: break;                         // RFIFG10
	case 24: break;                         // RFIFG11
	case 26: break;                         // RFIFG12
	case 28: break;                         // RFIFG13
	case 30: break;                         // RFIFG14
	case 32: break;                         // RFIFG15
	}
}