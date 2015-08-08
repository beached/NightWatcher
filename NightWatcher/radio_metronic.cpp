#include "radio_medtronic.h"
#include "radio_core.h"
#include <stdint.h>
#include "buffer.h"
#include "array.h"

size_t const radio_symbol_table_size = 54;
uint8_t const radio_symbol_table[radio_symbol_table_size] = { 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 11, 16, 13, 14, 16, 16, 16, 16, 16, 16, 0, 7, 16, 16, 9, 8, 16, 15, 16, 16, 16, 16, 16, 16, 3, 16, 5, 6, 16, 16, 16, 10, 16, 12, 16, 16, 16, 16, 1, 2, 16, 4 };

void radio_setup_916MHz( ) {
	// 	daw::radio::radio_write_single_reg( AGCCTRL0, 0x91 );
	// 	daw::radio::radio_write_single_reg( AGCCTRL1, 0x40 );
	// 	daw::radio::radio_write_single_reg( AGCCTRL2, 0x03 );
	// 	daw::radio::radio_write_single_reg( CHANNR, 0x02 );       //Channel Number
	// 	daw::radio::radio_write_single_reg( DEVIATN, 0x15 );      //Modem Deviation Setting
	// 	daw::radio::radio_write_single_reg( FOCCFG, 0x17 );       //Frequency Offset Compensation Configuration
	// 	daw::radio::radio_write_single_reg( FREND0, 0x11 );       //Front End TX Configuration	// DAW May be 0x12 from mmcommander
	// 	daw::radio::radio_write_single_reg( FREND1, 0x56 );
	// 	daw::radio::radio_write_single_reg( FREQ0, 0x00 );        //Frequency Control Word, Low Byte
	// 	daw::radio::radio_write_single_reg( FREQ1, 0x40 );        //Frequency Control Word, Middle Byte
	// 	daw::radio::radio_write_single_reg( FREQ2, 0x23 );        //Frequency Control Word, High Byte
	// 	daw::radio::radio_write_single_reg( FSCAL0, 0x1F );       //Frequency Synthesizer Calibration
	// 	daw::radio::radio_write_single_reg( FSCAL1, 0x00 );       //Frequency Synthesizer Calibration
	// 	daw::radio::radio_write_single_reg( FSCAL2, 0x2A );       //Frequency Synthesizer Calibration
	// 	daw::radio::radio_write_single_reg( FSCAL3, 0xE9 );       //Frequency Synthesizer Calibration
	// 	daw::radio::radio_write_single_reg( FSCTRL0, 0x0 );      //Frequency Synthesizer Control
	// 	daw::radio::radio_write_single_reg( FSCTRL1, 0x06 );      //Frequency Synthesizer Control
	// 	daw::radio::radio_write_single_reg( IOCFG0, 0x06 );       //GDO0 Output Configuration
	// 	daw::radio::radio_write_single_reg( LQI, 0xFF );          //Demodulator Estimate for Link Quality
	// 	daw::radio::radio_write_single_reg( MARCSTATE, 0x01 );    //Main Radio Control State Machine State
	// 	daw::radio::radio_write_single_reg( MCSM0, 0x10 );        //Main Radio Control State Machine Configuration
	// 	daw::radio::radio_write_single_reg( MDMCFG0, 0xC5 );      //Modem Configuration
	// 	daw::radio::radio_write_single_reg( MDMCFG1, 0x11 );      //Modem Configuration
	// 	daw::radio::radio_write_single_reg( MDMCFG2, 0x33 );      //Modem Configuration
	// 	daw::radio::radio_write_single_reg( MDMCFG3, 0x4A );      //Modem Configuration
	// 	daw::radio::radio_write_single_reg( MDMCFG4, 0x69 );      //Modem Configuration
	// 	daw::radio::radio_write_single_reg( PKTCTRL0, 0x00 );     //Packet Automation Control
	// 	daw::radio::radio_write_single_reg( PKTLEN, 0x10 );       //Packet Length
	// 	daw::radio::radio_write_single_reg( PKTSTATUS, 0xA0 );    //Current GDOx Status and Packet Status
	// 	daw::radio::radio_write_single_reg( TEST2, 0x88 );
	// 	daw::radio::radio_write_single_reg( TEST1, 0x31 );
	//
	// 	// 	RF1ADINW = 0xC002;   //Radio word data in register
	// 	// 	RF1AIE = 0x200;      //Radio core interrupt enable register
	// 	// 	RF1AIES = 0x200;     //Radio core interrupt edge select register
	// 	// 	RF1AIFCTL1 = 0x74;   //Radio interface control register 1
	// 	// 	RF1AIFERR = 0x01;    //Radio interface error flag register
	// 	// 	RF1AIFERRV = 0x02;   //Radio interface error vector word register
	// 	// 	RF1AIFG = 0x3E19;    //Radio core interrupt flag register
	// 	// 	RF1AIN = 0xC00;      //Radio core signal input register
	// 	// 	RF1AINSTR1W = 0xC000;//Radio instruction word register with 1-byte auto-r..
	// 	// 	RF1AINSTR2W = 0xC000;//Radio instruction word register with 2-byte auto-r..
	// 	// 	RF1AINSTRW = 0xC002; //Radio instruction word register
	// 	// 	RF1AIV = 0x14;       //Radio core interrupt vector word register
	// 	daw::radio::radio_write_single_reg( RSSI, 0xFC );         //Received Signal Strength Indication
	// 	daw::radio::radio_write_single_reg( SYNC0, 0x00 );        //Sync Word, Low Byte
	// 	daw::radio::radio_write_single_reg( SYNC1, 0xFF );        //Sync Word, High Byte
	// 	daw::radio::radio_write_single_reg( TEST0, 0x09 );        //Various Test Settings
	// 	daw::radio::radio_write_single_reg( VCO_VC_DAC, 0xFC );   //Current Setting from PLL Calibration Module
	// // 	daw::radio::radio_write_single_reg( WORCTRL, 0xF8 );      //Wake On Radio Control
	// // 	daw::radio::radio_write_single_reg( WOREVT0, 0x6B );      //Low Byte Event0 Timeout
	// // 	daw::radio::radio_write_single_reg( WOREVT1, 0x87 );      //High Byte Event0 Timeout
	//
	// Rf settings for CC430F6137
	//
	//
	// Rf settings for CC430F6137
	//
// 	daw::radio::radio_write_single_reg( IOCFG2, 0x29 );  //GDO2 Output Configuration
// 	daw::radio::radio_write_single_reg( IOCFG1, 0x2E );  //GDO1 Output Configuration
// 	daw::radio::radio_write_single_reg( IOCFG0, 0x06 );  //GDO0 Output Configuration
	daw::radio::radio_write_single_reg( FIFOTHR, 0x47 ); //RX FIFO and TX FIFO Thresholds
	daw::radio::radio_write_single_reg( SYNC1, 0xFF );   //Sync Word, High Byte
	daw::radio::radio_write_single_reg( SYNC0, 0x00 );   //Sync Word, Low Byte
	daw::radio::radio_write_single_reg( PKTLEN, 0xFF );  //Packet Length
	daw::radio::radio_write_single_reg( PKTCTRL1, 0x04 );//Packet Automation Control
	daw::radio::radio_write_single_reg( PKTCTRL0, 0x01 );//Packet Automation Control
	daw::radio::radio_write_single_reg( ADDR, 0x00 );    //Device Address
	daw::radio::radio_write_single_reg( CHANNR, 0x00 );  //Channel Number
	daw::radio::radio_write_single_reg( FSCTRL1, 0x06 ); //Frequency Synthesizer Control
	daw::radio::radio_write_single_reg( FSCTRL0, 0x00 ); //Frequency Synthesizer Control
	daw::radio::radio_write_single_reg( FREQ2, 0x23 );   //Frequency Control Word, High Byte
	daw::radio::radio_write_single_reg( FREQ1, 0x40 );   //Frequency Control Word, Middle Byte
	daw::radio::radio_write_single_reg( FREQ0, 0x19 );   //Frequency Control Word, Low Byte
	daw::radio::radio_write_single_reg( MDMCFG4, 0xC9 ); //Modem Configuration
	daw::radio::radio_write_single_reg( MDMCFG3, 0x4A ); //Modem Configuration
	daw::radio::radio_write_single_reg( MDMCFG2, 0x32 ); //Modem Configuration
	daw::radio::radio_write_single_reg( MDMCFG1, 0x11 ); //Modem Configuration
	daw::radio::radio_write_single_reg( MDMCFG0, 0xC5 ); //Modem Configuration
	daw::radio::radio_write_single_reg( DEVIATN, 0x15 ); //Modem Deviation Setting
	daw::radio::radio_write_single_reg( MCSM2, 0x07 );   //Main Radio Control State Machine Configuration
	daw::radio::radio_write_single_reg( MCSM1, 0x30 );   //Main Radio Control State Machine Configuration
	daw::radio::radio_write_single_reg( MCSM0, 0x10 );   //Main Radio Control State Machine Configuration
	daw::radio::radio_write_single_reg( FOCCFG, 0x17 );  //Frequency Offset Compensation Configuration
	daw::radio::radio_write_single_reg( AGCCTRL2, 0x03 );//AGC Control
	daw::radio::radio_write_single_reg( AGCCTRL1, 0x00 );//AGC Control
	daw::radio::radio_write_single_reg( AGCCTRL0, 0x91 );//AGC Control
// 	daw::radio::radio_write_single_reg( WOREVT1, 0x87 ); //High Byte Event0 Timeout
// 	daw::radio::radio_write_single_reg( WOREVT0, 0x6B ); //Low Byte Event0 Timeout
// 	daw::radio::radio_write_single_reg( WORCTRL, 0xF8 ); //Wake On Radio Control
	daw::radio::radio_write_single_reg( FREND1, 0x56 );  //Front End RX Configuration
	daw::radio::radio_write_single_reg( FREND0, 0x11 );  //Front End TX Configuration
	daw::radio::radio_write_single_reg( FSCAL3, 0xE9 );  //Frequency Synthesizer Calibration
	daw::radio::radio_write_single_reg( FSCAL2, 0x2A );  //Frequency Synthesizer Calibration
	daw::radio::radio_write_single_reg( FSCAL1, 0x00 );  //Frequency Synthesizer Calibration
	daw::radio::radio_write_single_reg( FSCAL0, 0x1F );  //Frequency Synthesizer Calibration
	daw::radio::radio_write_single_reg( TEST2, 0x81 );   //Various Test Settings
	daw::radio::radio_write_single_reg( TEST1, 0x35 );   //Various Test Settings
	daw::radio::radio_write_single_reg( TEST0, 0x09 );   //Various Test Settings

	{
		uint8_t const pa_values[8] = { 0x00,0x00,0x52,0x00,0x00,0x00,0x00,0x00 };
		daw::radio::radio_write_burst_pa_table( pa_values, 8 );
	}
}

radio_data_buffer_t radio_data_buffer;

namespace {
	void crc_init( uint8_t * const arry, size_t const & size_of ) {
		uint8_t const polynomial = 0x9b;
		uint8_t const msbit = 0x80;

		arry[0] = 0;
		uint8_t tmp = msbit;

		for( size_t i = 1; i < size_of; i *= 2 ) {
			uint8_t const p2 = (tmp & msbit) ? polynomial : 0;
			tmp = (tmp << 1) ^ p2;
			for( size_t j = 0; j < i; ++j ) {
				arry[i + j] = arry[j] ^ tmp;
			}
		}
	}
	daw::Array<uint8_t, 256> crc_table( crc_init );

	struct Packet {
		size_t data_start_idx;
		uint8_t length;
		uint8_t rssi;
		uint8_t packet_number;

		Packet( ) : data_start_idx( 0 ), length( 0 ), rssi( 0 ), packet_number( 0 ) { }
	};
	const uint8_t ERROR_DATA_BUFFER_OVERFLOW = 0x50;
	const uint8_t ERROR_TOO_MANY_PACKETS = 0x51;
	const uint8_t ERROR_RF_TX_OVERFLOW = 0x52;
	const uint8_t MAX_PACKET_SIZE = 250;
	const size_t MAX_PACKETS = 100;

	size_t packet_count = 0;
	size_t packet_head_idx = 0;

	daw::Buffer<Packet, MAX_PACKETS> packets;

	size_t data_buffer_bytes_used = 0;
	uint8_t buffer_overflow_count = 0;

	uint16_t symbol_input_buffer = 0;
	uint16_t symbol_input_bit_count = 0;
	uint16_t symbol_output_buffer = 0;
	size_t symbol_output_bit_count = 0;
	size_t symbol_error_count = 0;
	uint8_t packet_number = 0;
	uint8_t last_error = 0;
	uint8_t packet_overflow_count = 0;

	size_t buffer_write_pos = 0;
	//size_t buffer_read_pos = 0;

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

		size_t crc_read_idx = packets[packet_head_idx].data_start_idx;
		uint8_t crc_len = packets[packet_head_idx].length - 1;

		packets[packet_head_idx].rssi = daw::radio::radio_read_single_reg( RSSI );
		packets[packet_head_idx].packet_number = packet_number++;

		uint8_t crc = 0;
		while( crc_len-- > 0 ) {
			crc = crc_table[(crc ^ radio_data_buffer[crc_read_idx]) & 0xff];
			++crc_read_idx;
			if( radio_data_buffer.size( ) >= crc_read_idx ) {
				crc_read_idx = 0;
			}
		}
		//packet_crc = data_buffer[crc_read_idx];

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
	uint8_t symbol = 0;
	uint8_t output_sybmol = 0;

	if( 0 == value ) {
		if( 0 <= packets[packet_head_idx].length ) {
			finish_incoming_packet( );
		}
		return;
	}

	symbol_input_buffer = (symbol_input_buffer << 8) + value;
	symbol_input_bit_count += 8;

	while( 6 <= symbol_input_bit_count ) {
		symbol = (symbol_input_buffer >> (symbol_input_bit_count - 6) & 0b111111);
		symbol_input_bit_count -= 6;
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
		symbol_output_buffer = (symbol_output_buffer << 4) + symbol;
		symbol_output_bit_count += 4;
	}
	while( 8 <= symbol_output_bit_count ) {
		output_sybmol = (symbol_output_buffer >> (symbol_output_bit_count - 8)) & 0b11111111;
		symbol_output_bit_count -= 8;
		add_decoded_byte( output_sybmol );
	}

	if( 0 < symbol_error_count && 0 < packets[packet_head_idx].length ) {
		finish_incoming_packet( );
	}
}