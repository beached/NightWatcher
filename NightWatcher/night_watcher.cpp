#include "display.h"
#include "radio_core.h"
#include "radio_medtronic.h"
#include <cc430f6137.h>
#include "nullptr.h"

using std::size_t;

namespace {
	const uint8_t RX_TIMER_PERIOD = 85;

	inline void disable_watchdog( ) {
		WDTCTL = WDTPW | WDTHOLD;
	}

	void toggle_net_activity( ) {
		static uint8_t count = 1;
		if( count > 3 ) {
			count = 1;
		}
		switch( count ) {
		case 0:
			display::display_symbol( LCD_ICON_BEEPER3, display::LcdDisplayModes::SEG_OFF );
			break;
		case 1:
			display::display_symbol( LCD_ICON_BEEPER1, display::LcdDisplayModes::SEG_ON );
			break;
		case 2:
			display::display_symbol( LCD_ICON_BEEPER1, display::LcdDisplayModes::SEG_OFF );
			display::display_symbol( LCD_ICON_BEEPER2, display::LcdDisplayModes::SEG_ON );
			break;
		case 3:
			display::display_symbol( LCD_ICON_BEEPER2, display::LcdDisplayModes::SEG_OFF );
			display::display_symbol( LCD_ICON_BEEPER3, display::LcdDisplayModes::SEG_ON );
			break;
		}
	}

	daw::radio::RadioCore<256> radio;

	void setup_hardware( ) {
		daw::radio::set_vcore( 2u );
		daw::radio::init_fll( 8500000 / 1000, 8500000 / 32768 );

		display::lcd_init( );
		display::clear_display( );
		display::display_chars( LCD_SEG_LINE1_START, "On", display::LcdDisplayModes::SEG_ON );
		//init_timer( );
		radio.init_radio( radio_setup_916MHz );
	}

	typedef void( *state_function_ptr )();
	state_function_ptr current_state = nullptr;

	void state_waiting_for_interrupt( );
	void state_received_data( );
	void state_process_data( );
	void state_display_data( );
	void state_button_pushed( );

	void state_button_pushed( ) {
		current_state = state_waiting_for_interrupt;
	}

	void state_waiting_for_interrupt( ) {
		radio.receive_on( );
		_enable_interrupts( );
		__low_power_mode_2( );
		__no_operation( );
		if( radio.data_pending( ) ) {
			current_state = state_received_data;
		}
		/*
		if( button_push ) {
		current_state = state_button_pushed;
		}
		*/
	}

	void state_received_data( ) {
		size_t data_size = radio.receive_data( );
		if( 0 < data_size ) {
			current_state = state_process_data;
		} else {
			current_state = state_waiting_for_interrupt;
		}
	}

	void state_process_data( ) {
		if( radio.has_data( ) ) {
			toggle_net_activity( );
			receive_radio_symbols( radio.rx_array( ), radio.size( ) );
			radio.reset_rx_buffer( );
			current_state = state_display_data;
		} else {
			current_state = state_waiting_for_interrupt;
		}
	}

	void state_display_data( ) {
		// Allow radio traffic.  We are done with the radio buffer and
		radio.receive_on( );
		_enable_interrupts( );
		// Display glucose or something
		display::display_hex_chars( LCD_SEG_LINE1_START, (char const *)radio_data_buffer.data( ), display::LcdDisplayModes::SEG_ON );
		display::display_hex_chars( LCD_SEG_LINE2_START, (char const *)radio_data_buffer.data( ) + 4, display::LcdDisplayModes::SEG_ON );
		current_state = state_waiting_for_interrupt;
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