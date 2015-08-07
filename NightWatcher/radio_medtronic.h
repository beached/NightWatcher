#pragma once

#include <cstdint>

void radio_setup_916MHz( );
void receive_radio_symbol( uint8_t const & value );

template<typename ArrayType>
void receive_radio_symbols( ArrayType const & arry, std::size_t end_of_symbols ) {
	if( end_of_symbols > arry.size( ) ) {
		// TODO determine action if not ccover up as is, maybe error state?
		end_of_symbols = arry.size( );
	}
	for( std::size_t i = 0; i < end_of_symbols; ++i ) {
		receive_radio_symbol( arry[i] );
	}
}