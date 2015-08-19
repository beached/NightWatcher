
#include "display.h"
#include "radio_core.h"
#include "radio_config.h"
#include "night_watcher_state.h"

#include "radio_medtronic.h"

namespace {
	const uint16_t RX_TIMER_PERIOD = 40;

	inline void setup_timer( ) { }

	inline void setup_hardware( ) {
		daw::radio::core::set_vcore( 2u );
		daw::radio::core::init_fll( 8500000u / 1000u, 8500000u / 32768u );

		daw::display::lcd_init( );
		daw::display::clear_display( );
		display_chars( daw::display::defines::LCD_SEG_LINE1_START, "On", daw::display::SEG_ON ); //init_timer( );
		daw::radio::medtronic::radio.init_radio( daw::radio::medtronic::radio_setup_916MHz );
	}

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
	if( 2 == TA0IV && daw::radio::medtronic::radio.is_receiving( ) ) {
		if( daw::radio::medtronic::radio.is_receiving( ) ) {
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
		if( daw::radio::medtronic::radio.is_receiving( ) ) {
			daw::radio::medtronic::radio.check_for_data( );
			__no_operation( );
		} else {
			while( true ) {/* spin */ }	// trap
		}
		low_power_mode_off_on_exit( );
		break;
	}
}