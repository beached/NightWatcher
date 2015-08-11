#include "radio_medtronic.h"
#include "radio_core.h"
#include <stdint.h>
#include "buffer.h"
#include "array.h"

namespace daw {
	namespace radio {
		namespace medtronic {
			namespace {
				uint8_t const radio_symbol_table[] = { 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 11, 16, 13, 14, 16, 16, 16, 16, 16, 16, 0, 7, 16, 16, 9, 8, 16, 15, 16, 16, 16, 16, 16, 16, 3, 16, 5, 6, 16, 16, 16, 10, 16, 12, 16, 16, 16, 16, 1, 2, 16, 4 };
				size_t const radio_symbol_table_size = sizeof( radio_symbol_table );
			}

			radio_data_buffer_t radio_data_buffer;
			size_t symbol_error_count = 0;

			namespace {
				const uint8_t ERROR_DATA_BUFFER_OVERFLOW = 0x50;
				const uint8_t ERROR_TOO_MANY_PACKETS = 0x51;
				const uint8_t ERROR_RF_TX_OVERFLOW = 0x52;
				const uint8_t MAX_PACKET_SIZE = 250;
				const size_t MAX_PACKETS = 100;

				size_t packet_count = 0;
				size_t packet_head_idx = 0;
				size_t data_buffer_bytes_used = 0;
				uint8_t buffer_overflow_count = 0;
				uint16_t symbol_input_buffer = 0;
				uint16_t symbol_input_bit_count = 0;
				uint16_t symbol_output_buffer = 0;
				size_t symbol_output_bit_count = 0;
				uint8_t packet_number = 0;
				uint8_t last_error = 0;
				uint8_t packet_overflow_count = 0;
				size_t buffer_write_pos = 0;

				void crc_init( uint8_t * const arry, size_t const & size_of ) {
					uint8_t const msbit = 0x80;
					uint8_t const polynomial = 0x9b;

					auto tmp = msbit;
					arry[0] = 0;
					for( size_t i = 1; i < size_of; i *= 2 ) {
						auto const p2 = (tmp & msbit) ? polynomial : 0;
						tmp = (tmp << 1) ^ p2;
						for( size_t j = 0; j < i; ++j ) {
							arry[i + j] = arry[j] ^ tmp;
						}
					}
				}
				Array<uint8_t, 256> crc_table { crc_init };

				struct Packet {
					size_t data_start_idx;
					uint8_t length;
					uint8_t rssi;
					uint8_t packet_number;

					Packet( ): data_start_idx( 0 ), length( 0 ), rssi( 0 ), packet_number( 0 ) { }
				};

				Buffer<Packet, MAX_PACKETS> packets;

				void drop_current_packet( ) { }

				void add_decoded_byte( uint8_t const & value ) {
					if( radio_data_buffer.size( ) <= data_buffer_bytes_used ) {
						++buffer_overflow_count;
						drop_current_packet( );
						return;
					}
					radio_data_buffer[buffer_write_pos++] = value;
					++data_buffer_bytes_used;
					packets[packet_head_idx].length++;

					if( radio_data_buffer.size( ) <= buffer_write_pos ) {
						buffer_write_pos = 0;
					}

					if( MAX_PACKET_SIZE <= packets[packet_head_idx].length ) {
						drop_current_packet( );
					}
				}

				void reset_symbol_processing_state( ) {
					symbol_input_buffer = 0;
					symbol_input_bit_count = 0;
					symbol_output_buffer = 0;
					symbol_output_bit_count = 0;
					symbol_error_count = 0;
				}

				void finish_incoming_packet( ) {
					// 		uint16_t packet_crc = 0;
					packets[packet_head_idx].rssi = core::radio_read_single_reg( RSSI );
					packets[packet_head_idx].packet_number = packet_number++;
					{
						uint8_t crc = 0;
						auto crc_read_idx = packets[packet_head_idx].data_start_idx;
						for( uint8_t crc_len = packets[packet_head_idx].length - 1; crc_len > 0; --crc_len ) {
							crc = crc_table[(crc ^ radio_data_buffer[crc_read_idx]) & 0xff];
							++crc_read_idx;
							if( radio_data_buffer.size( ) >= crc_read_idx ) {
								crc_read_idx = 0;
							}
						}
						//packet_crc = data_buffer[crc_read_idx];
					}
					reset_symbol_processing_state( );

					if( MAX_PACKETS - 1 <= packet_count ) {
						// Packet count overflow
						last_error = ERROR_TOO_MANY_PACKETS;
						++packet_overflow_count;
						drop_current_packet( );
						return;
					}

					++packet_count;
					++packet_head_idx;
					if( MAX_PACKETS <= packet_head_idx ) {
						packet_head_idx = 0;
					}

					packets[packet_head_idx].data_start_idx = buffer_write_pos;
					packets[packet_head_idx].length = 0;
				}
			}	// namespace anonymous

			void receive_radio_symbol( uint8_t const & value ) {
				if( 0 == value ) {
					if( packets[packet_head_idx].length >= 0 ) {
						finish_incoming_packet( );
					}
					return;
				}

				symbol_input_buffer = (symbol_input_buffer << 8u) | value;
				symbol_input_bit_count += 8u;

				while( symbol_input_bit_count >= 6 ) {
					symbol_input_bit_count -= 6u;
					uint8_t symbol = static_cast<uint8_t>(symbol_input_buffer >> symbol_input_bit_count) & 0b00111111u;

					if( 0 == symbol ) {
						continue;
					}
					if( radio_symbol_table_size < symbol ) {
						++symbol_error_count;
						break;
					}
					symbol = radio_symbol_table[symbol];
					if( 16 == symbol ) {
						++symbol_error_count;
						break;
					}
					symbol_output_buffer = (symbol_output_buffer << 4u) | symbol;
					symbol_output_bit_count += 4;
				}
				while( symbol_output_bit_count >= 8 ) {
					symbol_output_bit_count -= 8;
					auto output_sybmol = static_cast<uint8_t>(symbol_output_buffer >> symbol_output_bit_count);
					add_decoded_byte( output_sybmol );
				}

				if( 0 < symbol_error_count && !packets.empty( ) && 0 < packets[packet_head_idx].length ) {
					finish_incoming_packet( );
				}
			}
		} // namespace medtronic
	} // namespace radio
}	// namespace daw