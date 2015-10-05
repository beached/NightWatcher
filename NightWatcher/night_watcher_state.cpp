#include <cc430f6137.h>
#include <intrinsics.h>
#include "night_watcher_state.h"
#include "radio_medtronic.h"
#include "radio_core.h"
#include "display.h"
#include "crc4b6b.h"
#include "packet_types.h"

using daw::radio::medtronic::radio;

namespace {
	class NetActivity {
		static void net_state_off( NetActivity & );
		static void net_state_1( NetActivity & );
		static void net_state_2( NetActivity & );
		static void net_state_3( NetActivity & );

		typedef void( *state_function_ptr )( NetActivity & );
		state_function_ptr state_function;
	public:
		NetActivity( )
			: state_function(net_state_off) { }

		void tick( ) {
			state_function( *this );
		}
	} net_activity;

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
}	// namespace active

ProgramState::ProgramState( )
	: state_function(state_waiting_for_interrupt) { }

void ProgramState::tick( ) {
	state_function( *this );
}


void ProgramState::state_button_pushed( ProgramState & self ) {
	self.state_function = state_waiting_for_interrupt;
}

void ProgramState::state_waiting_for_interrupt( ProgramState & self ) {
	daw::radio::medtronic::radio.receive_on( );
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

void ProgramState::state_determine_packet_type( ProgramState & self ) {
	uint8_t packet_type = 0xAA;
	switch( packet_type ) {
	case daw::radio::medtronic::packets::types::Glucometre: 
		self.state_function = state_process_glucometre_packet;
		break;
	case daw::radio::medtronic::packets::types::EnliteSensorWarmup:
	case daw::radio::medtronic::packets::types::EnliteSensor:
		self.state_function = state_process_sensor_packet;
		break;
	default:
		self.state_function = state_waiting_for_interrupt;
		break;
	}
}
	
void ProgramState::state_process_glucometre_packet( ProgramState & ) {
}
	
void ProgramState::state_process_sensor_packet( ProgramState & ) {
	
}

namespace {
	void Transmit( unsigned char *buffer, unsigned char length ) {
		RF1AIES |= BIT9;                          
		RF1AIFG &= ~BIT9;                         // Clear pending interrupts
		RF1AIE |= BIT9;                           // Enable TX end-of-packet interrupt
  
		WriteBurstReg( RF_TXFIFOWR, buffer, length );     
  
		Strobe( RF_STX );                         // Strobe STX   
	}
}
	
void ProgramState::state_process_data( ProgramState & self ) {
	if( radio.has_data( ) ) {
		daw::radio::medtronic::radio_data_buffer_size = daw::radio::medtronic::radio_data_buffer.size( );
		daw::radio::medtronic::crc4b6b::decode_4b6b( radio.rx_buffer( ).data( ), radio.size( ), daw::radio::medtronic::radio_data_buffer.data( ), daw::radio::medtronic::radio_data_buffer_size );
		radio.reset_rx_buffer( );
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
		display_hex_chars( defines::LCD_SEG_LINE1_START, static_cast<uint8_t const *>(daw::radio::medtronic::radio_data_buffer.data( )), SEG_ON );
		display_hex_chars( defines::LCD_SEG_LINE2_START, static_cast<uint8_t const *>(daw::radio::medtronic::radio_data_buffer.data( ) + 4), SEG_ON );
	} else {
		display_chars( defines::LCD_SEG_LINE1_START, "Err", SEG_ON );
	}
	radio.rx_buffer( ).clear( );
	self.state_function = state_waiting_for_interrupt;
}
