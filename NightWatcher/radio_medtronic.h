#pragma once

#include <stdint.h>
#include "buffer.h"

namespace daw {
	namespace radio {
		namespace medtronic {
			extern void radio_setup_916MHz( );
			extern void receive_radio_symbol( uint8_t const & value );

			typedef Buffer<uint8_t, 1024> radio_data_buffer_t;
			extern radio_data_buffer_t radio_data_buffer;

			template<typename ArrayType>
			void receive_radio_symbols( ArrayType const & arry, size_t end_of_symbols ) {
				if( end_of_symbols > arry.size( ) ) {
					// TODO determine action if not cover up as is, maybe error state?
					end_of_symbols = arry.size( );
				}
				for( size_t i = 0; i < end_of_symbols; ++i ) {
					receive_radio_symbol( arry[i] );
				}
			}
		} // namespace medtronic
	} // namespace radio
}	// namespace daw
