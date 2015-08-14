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
					const uint16_t s_crc16_table[256] = { 0x0, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7, 0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef, 0x1231, 0x210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6, 0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de, 0x2462, 0x3443, 0x420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485, 0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d, 0x3653, 0x2672, 0x1611, 0x630, 0x76d7, 0x66f6, 0x5695, 0x46b4, 0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc, 0x48c4, 0x58e5, 0x6886, 0x78a7, 0x840, 0x1861, 0x2802, 0x3823, 0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b, 0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0xa50, 0x3a33, 0x2a12, 0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a, 0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0xc60, 0x1c41, 0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49, 0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0xe70, 0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78, 0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f, 0x1080, 0xa1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067, 0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e, 0x2b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256, 0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d, 0x34e2, 0x24c3, 0x14a0, 0x481, 0x7466, 0x6447, 0x5424, 0x4405, 0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c, 0x26d3, 0x36f2, 0x691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634, 0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab, 0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x8e1, 0x3882, 0x28a3, 0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a, 0x4a75, 0x5a54, 0x6a37, 0x7a16, 0xaf1, 0x1ad0, 0x2ab3, 0x3a92, 0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9, 0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0xcc1, 0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8, 0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0xed1, 0x1ef0 };

					auto const s_crc8_table = []( ) {
						uint8_t const polynomial = 0x9b;
						Array<uint8_t, 256> result;
						result[0] = 0;

						// msb
						const unsigned char msbit = 0x80;
						uint8_t t = msbit;
						for( size_t i = 1; i < 256; i *= 2 ) {
							t = (t << 1) ^ (t & msbit ? polynomial : 0);
							for( size_t j = 0; j < i; j++ ) {
								result[i + j] = result[j] ^ t;
							}
						}
						return result;
					}();

					void crc8_part( uint8_t const & value, uint8_t & result ) {
						result = s_crc8_table[(result ^ value) & 0xFF];
					}

					void crc16_part( uint16_t const & value, uint16_t & result ) {
						auto const short_c = 0x00FF & static_cast<uint16_t>(value);
						auto const tmp = (result >> 8) ^ short_c;
						result = (result << 8) ^ s_crc16_table[tmp];
					}
				}

				daw::Array<uint8_t, 2> crc8( uint8_t const * const msg, size_t const & msg_size ) {
// 					static_assert(s_crc16_table.size( ) == 256, "CRC16 Table must have 256 items");
					assert( msg_size >= 2, "msg must be at least two bytes long" );
					daw::Array<uint8_t, 2> results;
					const auto & last_pos = msg_size - 1;

					for( size_t i = 0; i < last_pos; ++i ) {
						crc8_part( msg[i], results[1] );
					}
					results[0] = results[1];
					crc8_part( msg[last_pos], results[1] );

					return results;
				}

				daw::Array<uint16_t, 2> crc16( uint8_t const * const msg, size_t const & msg_size ) {
					static_assert(sizeof_array( s_crc16_table ) == 256, "CRC16 Table must have a length of 256");
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

				namespace {
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
				}	// namespace anonymous

				void encode_4b6b( uint8_t const * const in_message, size_t const & in_length, uint8_t * const out_message, size_t & out_length ) {
					size_t int_bits_available = 0;
					uint16_t int_buffer = 0;

					out_length = 0;
					for( size_t i = 0; i < in_length * 2; ++i ) {
						size_t j = i >> 1;
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
						uint16_t mask = (1 << int_bits_available) - 1;
						int_buffer = int_buffer & mask;
						int_buffer = int_buffer << (8 - int_bits_available);
						auto symbol = static_cast<uint8_t>(int_buffer & 0x00FF);
						out_message[out_length] = symbol;
						++out_length;
					}
				}

				void decode_4b6b( uint8_t const * const in_message, size_t const & in_length, uint8_t * const out_message, size_t & out_length ) {
					uint16_t int_buffer = 0;
					size_t int_bits_available = 0;
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