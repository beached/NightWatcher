#pragma once

#include <stdint.h>
#include <stddef.h>
#include "array.h"

namespace daw {
	namespace radio {
		namespace medtronic {
			namespace crc4b6b {
				extern daw::Array<uint8_t, 2> crc8( uint8_t const * const message, size_t const & msg_size );
				extern daw::Array<uint16_t, 2> crc16( uint8_t const * const msg, size_t const & msg_size );
				extern void encode_4b6b( uint8_t const * const in_message, size_t const & in_length, uint8_t * const out_message, size_t & out_length );
				extern void decode_4b6b( uint8_t const * const in_message, size_t const & in_length, uint8_t * const out_message, size_t & out_length );
			}	// namespace crc4b6b
		} // namespace medtronic
	} // namespace radio
}	// namespace daw
