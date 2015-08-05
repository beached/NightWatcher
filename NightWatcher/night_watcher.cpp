#include "display.h"
#include "radio_core.h"
#include "radio_medtronic.h"
#include <limits>
#include <cc430f6137.h>
#include "timers.h"
#include "registers.h"
const uint8_t RX_TIMER_PERIOD = 85;

namespace {
	inline void disable_watchdog( ) {
		WDTCTL = WDTPW | WDTHOLD;
	}

	//====================================================================
	/**
	* Startup routine for 32kHz Cristal on LFXT1
	*
	*/
	void LFXT_Start( uint16_t xtdrive ) {
		UCSCTL6_L |= XT1DRIVE1_L + XT1DRIVE0_L; // Highest drive setting for XT1 startup

		while( SFRIFG1 & OFIFG ) {   // check OFIFG fault flag
			UCSCTL7 &= ~(DCOFFG + XT1LFOFFG + XT1HFOFFG + XT2OFFG); // Clear OSC flaut Flags fault flags
			SFRIFG1 &= ~OFIFG;        // Clear OFIFG fault flag
		}
		UCSCTL6 = (UCSCTL6 & ~(XT1DRIVE_3)) | (xtdrive); // set Drive mode
	}

	void init_timer( ) {
		P5SEL |= 0x03;                            // Set xtal pins
		LFXT_Start( XT1DRIVE_0 );

		TA0CCR1 = RX_TIMER_PERIOD;               // x cycles * 1/32768 = y us
		TA0CCTL1 = CCIE;                          // Enable interrupts
		TA0CTL = TASSEL__ACLK + TACLR;          // ACLK source
	}
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

static daw::radio::RadioCore<256> radio;

int main( ) {
	// Setup
	disable_watchdog( );
	daw::radio::set_vcore( 3u );
	daw::radio::init_fll( 8500000 / 1000, 8500000 / 32768 );

	display::lcd_init( );
	display::clear_display( );
	display::display_chars( display::LCD_SEG_LINE1_START, "On", display::LcdDisplayModes::SEG_ON );
	//init_timer( );
	uint32_t data_counter = 0;
	radio.init_radio( radio_setup_916MHz );
	toggle_net_activity( );
	while( true ) {
		radio.receive_on( );
		_enable_interrupts( );
		//__low_power_mode_2( );
		__no_operation( );
		if( radio.data_pending( ) ) {
			toggle_net_activity( );
			while( radio.receive_data( ) > 0 ) {
				toggle_net_activity( );
				if( radio.has_data( ) ) {
					__no_operation( );
				}
			}
		}
	}
}

// __attribute__( (interrupt( TIMER0_A1_VECTOR )) ) void timer_isr( ) {
// 	switch( __even_in_range( TA0IV, 14 ) ) {
// 	case 0:  break;
// 	case 2:
// 		TA0CCR1 += RX_TIMER_PERIOD;                  // 16 cycles * 1/32768 = ~500 us
// 		//rx_packet_handler( );
// 		if( radio.is_receiving( ) ) {
// 			radio.check_for_data( );
// 			__no_operation( );
// 		}
// 		if( radio.has_data( ) ) {
// 			_low_power_mode_off_on_exit( );
// 		}
// 		break;
// 	case 4:  break;                         // CCR2 not used
// 	case 6:  break;                         // Reserved not used
// 	case 8:  break;                         // Reserved not used
// 	case 10: break;                         // Reserved not used
// 	case 12: break;                         // Reserved not used
// 	case 14: break;                         // Overflow not used
// 	}
// }

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