#pragma once

#define low_power_mode( ) _BIS_SR( LPM3_bits + GIE)
#define low_power_mode_off_on_exit( ) LPM3_EXIT
#define disable_watchdog( ) WDTCTL = WDTPW | WDTHOLD

class ProgramState {
	typedef void( *state_function_ptr )( ProgramState & );
	state_function_ptr state_function;

	static void state_waiting_for_interrupt( ProgramState & );
	static void state_received_data( ProgramState & );
	static void state_process_data( ProgramState & );
	static void state_display_data( ProgramState & );
	static void state_button_pushed( ProgramState & );
	static void state_determine_packet_type( ProgramState & );
	static void state_process_glucometre_packet( ProgramState & );
	static void state_process_sensor_packet( ProgramState & );
public:
	ProgramState( );
	void tick( );
};


