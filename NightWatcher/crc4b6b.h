#pragma once

#include <stdint.h>
#include <stddef.h>

namespace daw {
	namespace radio {
		namespace medtronic {
			namespace crc4b6b {
				extern uint8_t crc8( uint8_t const msg[], size_t const & msg_size );
				extern uint16_t crc16( uint8_t const msg[], size_t const & msg_size );
				extern void encode_4b6b( uint8_t const in_message[], size_t const & in_length, uint8_t out_message[], size_t & out_length );
				extern void decode_4b6b( uint8_t const in_message[], size_t const & in_length, uint8_t out_message[], size_t & out_length );
			}	// namespace crc4b6b
		} // namespace medtronic
	} // namespace radio
}	// namespace daw
