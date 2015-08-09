#include "crc4b6b.h"
#include <stdint.h>
#include "array.h"

namespace daw {
	namespace radio {
		namespace medtronic {
			namespace crc4b6b {
				namespace {
					daw::Array<uint16_t, 256> crc16_table;
				}

				uint8_t crc8( uint8_t *message, size_t length ) {
					uint8_t remainder = 0;

					for( size_t byte = 0; byte < length; ++byte ) {
						remainder ^= message[byte];
						for( size_t bit = 8; bit > 0; --bit ) {
							if( remainder & 0x80 ) {
								remainder = (remainder << 1) ^ 0x9B;
							} else {
								remainder = (remainder << 1);
							}
						}
					}
					return remainder;
				}

				void crc16_init( ) {
					uint16_t j = 0;
					uint16_t crc = 0;
					uint16_t c = 0;

					for( size_t i = 0; i < 256; i++ ) {
						crc = 0;
						c = ((uint16_t)i) << 8;
						for( j = 0; j < 8; j++ ) {
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

				int16_t crc16( uint8_t *message, size_t length ) {
					uint16_t crc = 0xFFFF;
					uint16_t short_c = 0;
					uint16_t tmp = 0;
					for( size_t i = 0; i < length; i++ ) {
						short_c = 0x00FF & static_cast<uint16_t>(message[i]);
						tmp = (crc >> 8) ^ short_c;
						crc = (crc << 8) ^ crc16_table[tmp];
					}
					return crc;
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
					uint16_t j = 0;
					int16_t int_buffer = 0;
					int16_t mask = 0;
					int16_t int_bits_available = 0;
					uint8_t symbol = 0;

					out_length = 0;

					for( size_t i = 0; i < in_length * 2; ++i ) {
						j = i >> 1;
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
							mask = (1 << int_bits_available) - 1;
							int_buffer = int_buffer & mask;
						}
					}

					if( int_bits_available != 0 ) {
						mask = (1 << int_bits_available) - 1;
						int_buffer = int_buffer & mask;
						int_buffer = int_buffer << (8 - int_bits_available);
						symbol = int_buffer & 0x00FF;
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