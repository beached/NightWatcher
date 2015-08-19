#include "radio_config.h"
#include "radio_core.h"

namespace daw {
	namespace radio {
		namespace medtronic {
			void radio_setup_916MHz( ) {
				using namespace core;
//
// Rf settings for CC430F6137
//
				radio_write_single_reg( IOCFG0, 0x06 );       //GDO0 Output Configuration
				radio_write_single_reg( FIFOTHR, 0x47 );      //RX FIFO and TX FIFO Thresholds
				radio_write_single_reg( SYNC1, 0x00 );        //Sync Word, High Byte
				radio_write_single_reg( SYNC0, 0xFF );        //Sync Word, Low Byte
				radio_write_single_reg( PKTLEN, 0x35 );       //Packet Length
				radio_write_single_reg( PKTCTRL0, 0x00 );     //Packet Automation Control
				radio_write_single_reg( FSCTRL1, 0x06 );      //Frequency Synthesizer Control
				radio_write_single_reg( FREQ2, 0x23 );        //Frequency Control Word, High Byte
				radio_write_single_reg( FREQ1, 0x40 );        //Frequency Control Word, Middle Byte
				radio_write_single_reg( FREQ0, 0x00 );        //Frequency Control Word, Low Byte
				radio_write_single_reg( MDMCFG4, 0x89 );      //Modem Configuration
				radio_write_single_reg( MDMCFG3, 0x4A );      //Modem Configuration
				radio_write_single_reg( MDMCFG2, 0x31 );      //Modem Configuration
				radio_write_single_reg( MDMCFG1, 0x11 );      //Modem Configuration
				radio_write_single_reg( MDMCFG0, 0xC5 );      //Modem Configuration
				radio_write_single_reg( DEVIATN, 0x15 );      //Modem Deviation Setting
				radio_write_single_reg( MCSM0, 0x10 );        //Main Radio Control State Machine Configuration
				radio_write_single_reg( FOCCFG, 0x17 );       //Frequency Offset Compensation Configuration
				radio_write_single_reg( AGCCTRL1, 0x00 );     //AGC Control
				radio_write_single_reg( WOREVT1, 0x87 );      //High Byte Event0 Timeout
				radio_write_single_reg( WOREVT0, 0x6B );      //Low Byte Event0 Timeout
				radio_write_single_reg( WORCTRL, 0xF8 );      //Wake On Radio Control
				radio_write_single_reg( FREND0, 0x11 );       //Front End TX Configuration
				radio_write_single_reg( FSCAL3, 0xE9 );       //Frequency Synthesizer Calibration
				radio_write_single_reg( FSCAL2, 0x2A );       //Frequency Synthesizer Calibration
				radio_write_single_reg( FSCAL1, 0x00 );       //Frequency Synthesizer Calibration
				radio_write_single_reg( FSCAL0, 0x1F );       //Frequency Synthesizer Calibration
				radio_write_single_reg( TEST2, 0x81 );        //Various Test Settings
				radio_write_single_reg( TEST1, 0x35 );        //Various Test Settings
				radio_write_single_reg( TEST0, 0x09 );        //Various Test Settings
				radio_write_single_reg( LQI, 0x7F );          //Demodulator Estimate for Link Quality
				radio_write_single_reg( RSSI, 0x80 );         //Received Signal Strength Indication
				radio_write_single_reg( VCO_VC_DAC, 0x94 );   //Current Setting from PLL Calibration Module
				{
					uint8_t const pa_values[8] = { 0x00, 0x00, 0x52, 0x00, 0x00, 0x00, 0x00, 0x00 };
					radio_write_burst_pa_table( pa_values, 8 );
				}
			}
		} // namespace medtronic
	} // namespace radio
}	// namespace daw