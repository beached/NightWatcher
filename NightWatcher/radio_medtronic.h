#pragma once

#include <stdint.h>
#include "buffer.h"
#include "intrinsics.h"

namespace daw {
	namespace radio {
		namespace medtronic {
			extern void receive_radio_symbol( uint8_t const & value );

			typedef Buffer<uint8_t, 1024> radio_data_buffer_t;
			extern radio_data_buffer_t radio_data_buffer;
			extern size_t symbol_error_count;

			template<typename BufferType>
			void receive_radio_symbols( BufferType const & rdo_buffer ) {
				for( auto const & item : rdo_buffer ) {
					receive_radio_symbol( item );
				}
				__no_operation( );
			}
		} // namespace medtronic
	} // namespace radio
}	// namespace daw
