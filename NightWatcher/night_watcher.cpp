#include <cc430f6137.h>
#include <intrinsics.h>

#include "display.h"
#include "radio_core.h"
#include "radio_medtronic.h"
#include "radio_config.h"

#define low_power_mode( ) _BIS_SR( LPM3_bits + GIE)
#define low_power_mode_off_on_exit( ) LPM3_EXIT;
#define disable_watchdog( ) WDTCTL = WDTPW | WDTHOLD;
namespace {
	const uint16_t RX_TIMER_PERIOD = 40;

#pragma region NetActivity
	class NetActivity {
		static void net_state_off( NetActivity & );
		static void net_state_1( NetActivity & );
		static void net_state_2( NetActivity & );
		static void net_state_3( NetActivity & );

		typedef void( *state_function_ptr )(NetActivity &);
		state_function_ptr state_function;
	public:
		NetActivity( ): state_function( net_state_off ) { }

		void tick( ) {
			state_function( *this );
		}
	};

	void NetActivity::net_state_off( NetActivity & self ) {
		display_symbol( daw::display::defines::LCD_ICON_BEEPER1, daw::display::SEG_OFF );
		display_symbol( daw::display::defines::LCD_ICON_BEEPER2, daw::display::SEG_OFF );
		display_symbol( daw::display::defines::LCD_ICON_BEEPER3, daw::display::SEG_OFF );
		self.state_function = net_state_1;
	}

	void NetActivity::net_state_1( NetActivity & self ) {
		display_symbol( daw::display::defines::LCD_ICON_BEEPER1, daw::display::SEG_ON );
		display_symbol( daw::display::defines::LCD_ICON_BEEPER2, daw::display::SEG_OFF );
		display_symbol( daw::display::defines::LCD_ICON_BEEPER3, daw::display::SEG_OFF );
		self.state_function = net_state_2;
	}

	void NetActivity::net_state_2( NetActivity & self ) {
		display_symbol( daw::display::defines::LCD_ICON_BEEPER1, daw::display::SEG_OFF );
		display_symbol( daw::display::defines::LCD_ICON_BEEPER2, daw::display::SEG_ON );
		display_symbol( daw::display::defines::LCD_ICON_BEEPER3, daw::display::SEG_OFF );
		self.state_function = net_state_3;
	}

	void NetActivity::net_state_3( NetActivity & self ) {
		display_symbol( daw::display::defines::LCD_ICON_BEEPER1, daw::display::SEG_OFF );
		display_symbol( daw::display::defines::LCD_ICON_BEEPER2, daw::display::SEG_OFF );
		display_symbol( daw::display::defines::LCD_ICON_BEEPER3, daw::display::SEG_ON );
		self.state_function = net_state_1;
	}
#pragma endregion

	daw::radio::core::RadioCore<64> radio;
	NetActivity net_activity;

	inline void setup_timer( ) { }

	inline void setup_hardware( ) {
		daw::radio::core::set_vcore( 2u );
		daw::radio::core::init_fll( 8500000u / 1000u, 8500000u / 32768u );

		daw::display::lcd_init( );
		daw::display::clear_display( );
		display_chars( daw::display::defines::LCD_SEG_LINE1_START, "On", daw::display::SEG_ON ); //init_timer( );
		radio.init_radio( daw::radio::medtronic::radio_setup_916MHz );
	}

#pragma region ProgramState
	class ProgramState {
		typedef void( *state_function_ptr )(ProgramState &);
		state_function_ptr state_function;

		static void state_waiting_for_interrupt( ProgramState & );
		static void state_received_data( ProgramState & );
		static void state_process_data( ProgramState & );
		static void state_display_data( ProgramState & );
		static void state_button_pushed( ProgramState & );
	public:
		ProgramState( ): state_function( state_waiting_for_interrupt ) { }

		void tick( ) {
			state_function( *this );
		}
	};

	void ProgramState::state_button_pushed( ProgramState & self ) {
		self.state_function = state_waiting_for_interrupt;
	}

	void ProgramState::state_waiting_for_interrupt( ProgramState & self ) {
		radio.receive_on( );
		__enable_interrupt( );
		low_power_mode( );	// This will put MCU to sleep and wait for interrupt
		__no_operation( );
		if( radio.data_pending( ) ) {
			self.state_function = state_received_data;
		}
		/*
		if( button_push ) {
		current_state = state_button_pushed;
		}
		*/
	}

	void ProgramState::state_received_data( ProgramState & self ) {
		net_activity.tick( );
		auto const data_size = radio.receive_data( );
		if( data_size > 0 ) {
			self.state_function = state_process_data;
		} else {
			self.state_function = state_waiting_for_interrupt;
		}
	}

	void ProgramState::state_process_data( ProgramState & self ) {
		if( radio.has_data( ) ) {
			daw::radio::medtronic::receive_radio_symbols( radio.rx_buffer( ) );
			//radio.reset_rx_buffer( );
			self.state_function = state_display_data;
		} else {
			self.state_function = state_waiting_for_interrupt;
		}
	}

	void ProgramState::state_display_data( ProgramState & self ) {
		// Allow radio traffic.  We are done with the radio buffer and
		using namespace daw::display;
		// Display glucose or something
		if( !radio.rx_buffer( ).empty( ) ) {
			display_hex_chars( defines::LCD_SEG_LINE1_START, static_cast<uint8_t const *>(radio.rx_buffer( ).data( )), SEG_ON );
			display_hex_chars( defines::LCD_SEG_LINE2_START, static_cast<uint8_t const *>(radio.rx_buffer( ).data( ) + 4), SEG_ON );
		} else {
			display_chars( defines::LCD_SEG_LINE1_START, "Err", SEG_ON );
			display_value( defines::LCD_SEG_LINE2_START, daw::radio::medtronic::symbol_error_count, 5, 0, SEG_ON );
		}
		radio.rx_buffer( ).clear( );
		self.state_function = state_waiting_for_interrupt;
	}
#pragma endregion
}	// namespace anonymous

static bool volatile packet_received = false;

int main( ) {
	// Setup
	disable_watchdog( );
	ProgramState program_state;
	setup_hardware( );

	while( true ) {
		program_state.tick( );
	}
}

void packet_handler( ) { }

void __attribute__( (interrupt( TIMER0_A1_VECTOR )) ) timer_radio_status_isr( ) {
	if( 2 == TA0IV && radio.is_receiving( ) ) {
		if( radio.is_receiving( ) ) {
			TA0CCR1 += RX_TIMER_PERIOD;                  // 16 cycles * 1/32768 = ~500 us

			packet_handler( );

			if( packet_received ) {
				packet_received = false;
				__bic_SR_register_on_exit( LPM3_bits );
			}
		}
	}
}

void __attribute__( (interrupt( CC1101_VECTOR )) ) radio_isr( ) {
	switch( RF1AIV ) {	// Prioritizing Radio Core Interrupt
	case  4:								// RFIFG1
		RF1AIE &= ~(BIT1 | BIT9);
		daw::radio::core::strobe( RF_SWOR ); // Go back to sleep
		P1OUT ^= BIT0;
		break;
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
	}
}