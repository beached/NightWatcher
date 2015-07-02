#pragma once

#include <cc430f6137.h>
#include <cstdint>
#include <array>

class Radio {
public:
	static volatile bool receive_loop_on;
	Radio( );
	// 	void receive_data( ) const;
	// 	void receive_on( ) const;
	// 	void receive_off( ) const;
	static uint8_t strobe( uint8_t cmd );
	static void reset_core( );
	static void receive_loop( );
	static void receive_on( );
};
