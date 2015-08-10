#include <cc430f6137.h>
#include <intrinsics.h>

#include "display.h"
#include "radio_core.h"
#include "radio_medtronic.h"

#define low_power_mode( ) _BIS_SR( LPM3_bits + GIE)
#define low_power_mode_off_on_exit( ) LPM3_EXIT;
namespace {
	inline void disable_watchdog( ) {
		WDTCTL = WDTPW | WDTHOLD;
	}

#pragma region NetActivity
	class NetActivity {
		static void net_state_off( NetActivity & );
		static void net_state_1( NetActivity & );
		static void net_state_2( NetActivity & );
		static void net_state_3( NetActivity & );

		typedef void( *net_state_fn )(NetActivity &);
		net_state_fn m_current_net_state;
	public:
		NetActivity( ): m_current_net_state( net_state_off ) { }

		void current_state( ) {
			m_current_net_state( *this );
		}
	};

	void NetActivity::net_state_off( NetActivity & self ) {
		display_symbol( daw::display::defines::LCD_ICON_BEEPER1, daw::display::SEG_OFF );
		display_symbol( daw::display::defines::LCD_ICON_BEEPER2, daw::display::SEG_OFF );
		display_symbol( daw::display::defines::LCD_ICON_BEEPER3, daw::display::SEG_OFF );
		self.m_current_net_state = net_state_1;
	}

	void NetActivity::net_state_1( NetActivity & self ) {
		display_symbol( daw::display::defines::LCD_ICON_BEEPER1, daw::display::SEG_ON );
		display_symbol( daw::display::defines::LCD_ICON_BEEPER2, daw::display::SEG_OFF );
		display_symbol( daw::display::defines::LCD_ICON_BEEPER3, daw::display::SEG_OFF );
		self.m_current_net_state = net_state_2;
	}

	void NetActivity::net_state_2( NetActivity & self ) {
		display_symbol( daw::display::defines::LCD_ICON_BEEPER1, daw::display::SEG_OFF );
		display_symbol( daw::display::defines::LCD_ICON_BEEPER2, daw::display::SEG_ON );
		display_symbol( daw::display::defines::LCD_ICON_BEEPER3, daw::display::SEG_OFF );
		self.m_current_net_state = net_state_3;
	}

	void NetActivity::net_state_3( NetActivity & self ) {
		display_symbol( daw::display::defines::LCD_ICON_BEEPER1, daw::display::SEG_OFF );
		display_symbol( daw::display::defines::LCD_ICON_BEEPER2, daw::display::SEG_OFF );
		display_symbol( daw::display::defines::LCD_ICON_BEEPER3, daw::display::SEG_ON );
		self.m_current_net_state = net_state_1;
	}
#pragma endregion 
	daw::radio::core::RadioCore<256> radio;
	NetActivity net_activity;

	void setup_hardware( ) {
		daw::radio::core::set_vcore( 2u );
		daw::radio::core::init_fll( 8500000 / 1000, 8500000 / 32768 );

		daw::display::lcd_init( );
		daw::display::clear_display( );
		display_chars( daw::display::defines::LCD_SEG_LINE1_START, "On", daw::display::SEG_ON ); //init_timer( );
		radio.init_radio( daw::radio::medtronic::radio_setup_916MHz );
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
		__enable_interrupt( );
		low_power_mode( );
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
		net_activity.current_state( );
		auto const data_size = radio.receive_data( );
		if( 0 < data_size ) {
			current_state = state_process_data;
		} else {
			current_state = state_waiting_for_interrupt;
		}
	}

	void state_process_data( ) {
		if( radio.has_data( ) ) {
			daw::radio::medtronic::receive_radio_symbols( radio.rx_array( ), radio.size( ) );
			radio.reset_rx_buffer( );
			current_state = state_display_data;
		} else {
			current_state = state_waiting_for_interrupt;
		}
	}

	void state_display_data( ) {
		// Allow radio traffic.  We are done with the radio buffer and
		radio.receive_on( );
		__enable_interrupt( ); // Display glucose or something
		display_hex_chars( daw::display::defines::LCD_SEG_LINE1_START, static_cast<uint8_t const *>(daw::radio::medtronic::radio_data_buffer.data( )), daw::display::SEG_ON );
		display_hex_chars( daw::display::defines::LCD_SEG_LINE2_START, static_cast<uint8_t const *>(daw::radio::medtronic::radio_data_buffer.data( ) + 4), daw::display::SEG_ON );
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

#define __even_in_range( x, y ) (x)

void __attribute__( (interrupt( CC1101_VECTOR )) ) radio_isr( ) {
	switch( __even_in_range( RF1AIV, 32 ) ) {	// Prioritizing Radio Core Interrupt
	case  0: break; // No RF core interrupt pending
	case  2: break; // RFIFG0
	case  4:								// RFIFG1
		RF1AIE &= ~(BIT1 | BIT9);
		daw::radio::core::strobe( RF_SWOR ); // Go back to sleep
		P1OUT ^= BIT0;
		break;
	case  6: break; // RFIFG2
	case  8: break; // RFIFG3
	case 10: break; // RFIFG4
	case 12: break; // RFIFG5
	case 14: break; // RFIFG6
	case 16: break; // RFIFG7
	case 18: break; // RFIFG8
	case 20: // RFIFG9
		RF1AIE &= ~(BIT1 | BIT9);
		if( radio.is_receiving( ) ) {
			radio.check_for_data( );
			__no_operation( );
		} else {
			while( true ) {/* spin */ }	// trap
		}
		low_power_mode_off_on_exit( );
		break;
	case 22: break; // RFIFG10
	case 24: break; // RFIFG11
	case 26: break; // RFIFG12
	case 28: break; // RFIFG13
	case 30: break; // RFIFG14
	case 32: break; // RFIFG15
	}
}