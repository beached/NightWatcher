#pragma once

#include <cc430f6137.h>
#include <cstdint>
#include <array>

class Radio {
public:
	Radio( );
	void receive_data( );
	void receive_on( );
	void receive_off( );
private:
	std::array<uint8_t, 1024> m_rx_buffer;
	uint8_t strobe( uint8_t cmd );
	void reset_core( );
};
