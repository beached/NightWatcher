#include "crc4b6b.h"
#include <stdint.h>
#include <stddef.h>
#include "array.h"
#include "assert.h"

namespace daw {
	namespace radio {
		namespace medtronic {
			namespace crc4b6b {
				namespace {
					Array<uint16_t, 256> crc16_table;
				}

				namespace {
					void crc8_part( uint8_t const & value, uint8_t & result ) {
						result ^= value;
						for( size_t bit = 8; bit > 0; --bit ) {
							result = (result << 1);
							if( result & 0x80 ) {
								result ^= 0x9B;
							}
						}
					}
				}

				daw::Array<uint8_t, 2> crc8( uint8_t const * const msg, size_t const & msg_size ) {
					assert( msg_size >= 2, "msg must be at least two bytes long" );
					daw::Array<uint8_t, 2> results;
					for( size_t i = 0; i < msg_size - 1; ++i ) {
						crc8_part( msg[i], results[1] );
					}
					results[0] = results[1];
					crc8_part( msg[msg_size - 1], results[1] );

					return results;
				}

				namespace {
					void crc16_part( uint16_t const & value, uint16_t & result ) {
						auto const short_c = 0x00FF & static_cast<uint16_t>(value);
						auto const tmp = (result >> 8) ^ short_c;
						result = (result << 8) ^ crc16_table[tmp];
					}
				}

				daw::Array<uint16_t, 2> crc16( uint8_t const * const msg, size_t const & msg_size ) {
					static_assert(crc16_table.size( ) == 256, "CRC16 Table must have a length of 256");
					assert( msg_size >= 2, "msg must be at least two bytes long" );
					daw::Array<uint16_t, 2> results;
					results[1] = 0xFFFF;
					for( size_t i = 0; i < msg_size - 1; i++ ) {
						crc16_part( msg[i], results[1] );
					}
					results[0] = results[1];
					crc16_part( msg[msg_size - 1], results[1] );
					return results;
				}

				void crc16_init( ) {
					static_assert(crc16_table.size( ) == 256, "CRC16 Table must have a length of 256");
					for( size_t i = 0; i < 256; i++ ) {
						uint16_t crc = 0;
						auto c = static_cast<uint16_t>(i) << 8;
						for( size_t j = 0; j < 8; j++ ) {
							if( (crc^c) & 0x8000 ) {
								crc = (crc << 1) ^ 0x1021;
							} else {
								crc = crc << 1;
							}
							c = c << 1;
						}
						crc16_table[i] = crc;
					}
				}

				uint8_t get_4b6b_symbol( uint8_t const & in_symbol ) {
					switch( in_symbol ) {
					case 0x00:
						return 0x15;
					case 0x01:
						return 0x31;
					case 0x02:
						return 0x32;
					case 0x03:
						return 0x23;
					case 0x04:
						return 0x34;
					case 0x05:
						return 0x25;
					case 0x06:
						return 0x26;
					case 0x07:
						return 0x16;
					case 0x08:
						return 0x1A;
					case 0x09:
						return 0x19;
					case 0x0A:
						return 0x2A;
					case 0x0B:
						return 0x0B;
					case 0x0C:
						return 0x2C;
					case 0x0D:
						return 0x0D;
					case 0x0E:
						return 0x0E;
					case 0x0F:
						return 0x1C;
					default:
						return 0x00;
					}
				}

				uint8_t decode_4b6b_symbol( uint8_t const & in_symbol, uint8_t & out_symbol ) {
					switch( in_symbol ) {
					case 0x15:
						out_symbol = 0x00;
						return 0;
					case 0x31:
						out_symbol = 0x01;
						return 0;
					case 0x32:
						out_symbol = 0x02;
						return 0;
					case 0x23:
						out_symbol = 0x03;
						return 0;
					case 0x34:
						out_symbol = 0x04;
						return 0;
					case 0x25:
						out_symbol = 0x05;
						return 0;
					case 0x26:
						out_symbol = 0x06;
						return 0;
					case 0x16:
						out_symbol = 0x07;
						return 0;
					case 0x1A:
						out_symbol = 0x08;
						return 0;
					case 0x19:
						out_symbol = 0x09;
						return 0;
					case 0x2A:
						out_symbol = 0x0A;
						return 0;
					case 0x0B:
						out_symbol = 0x0B;
						return 0;
					case 0x2C:
						out_symbol = 0x0C;
						return 0;
					case 0x0D:
						out_symbol = 0x0D;
						return 0;
					case 0x0E:
						out_symbol = 0x0E;
						return 0;
					case 0x1C:
						out_symbol = 0x0F;
						return 0;
					default:
						out_symbol = 0x00;
						return 1;
					}
				}

				void encode_4b6b( uint8_t *in_message, size_t in_length, uint8_t *out_message, size_t & out_length ) {
					int16_t int_bits_available = 0;
					int16_t int_buffer = 0;

					out_length = 0;
					for( size_t i = 0; i < in_length * 2; ++i ) {
						uint16_t j = i >> 1;
						uint8_t symbol = 0;
						if( j * 2 == i ) {
							symbol = (in_message[j] >> 4) & 0x0F;
						} else {
							symbol = in_message[j] & 0x0F;
						}
						symbol = get_4b6b_symbol( symbol );
						int_buffer = (int_buffer << 6) & 0xFFC0;
						int_buffer = int_buffer | symbol;
						int_bits_available += 6;
						if( int_bits_available >= 8 ) {
							symbol = int_buffer >> (int_bits_available - 8);
							out_message[out_length] = symbol;
							++out_length;
							int_bits_available -= 8;
							int16_t mask = (1 << int_bits_available) - 1;
							int_buffer = int_buffer & mask;
						}
					}

					if( int_bits_available != 0 ) {
						int16_t mask = (1 << int_bits_available) - 1;
						int_buffer = int_buffer & mask;
						int_buffer = int_buffer << (8 - int_bits_available);
						auto symbol = static_cast<uint8_t>(int_buffer & 0x00FF);
						out_message[out_length] = symbol;
						++out_length;
					}
				}

				void decode_4b6b( uint8_t *in_message, size_t const & in_length, uint8_t *out_message, size_t & out_length ) {
					int16_t int_buffer = 0;
					int16_t int_bits_available = 0;
					uint8_t symbol = 0;
					uint8_t rec_byte = 0;
					uint8_t nibble_flag = 0;
					size_t out_bytes = 0;

					for( size_t i = 0; i < in_length; i++ ) {
						int_buffer = int_buffer << 8;
						int_buffer |= in_message[i];
						int_bits_available += 8;

						while( int_bits_available >= 6 ) {
							symbol = int_buffer >> (int_bits_available - 6);
							symbol &= 0x3F;
							decode_4b6b_symbol( symbol, symbol );
							rec_byte = rec_byte << 4;
							rec_byte &= 0xF0;
							rec_byte |= symbol;
							nibble_flag ^= 1;
							if( nibble_flag == 0 ) {
								out_message[out_bytes] = rec_byte;
								out_bytes++;
							}
							int_bits_available -= 6;
						}
					}

					out_length = out_bytes;
				}
			}	// namespace crc4b6b
		} // namespace medtronic
	} // namespace radio
}	// namespace daw