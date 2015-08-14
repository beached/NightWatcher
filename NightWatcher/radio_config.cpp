#include "radio_config.h"
#include "radio_core.h"

namespace daw {
	namespace radio {
		namespace medtronic {
			void radio_setup_916MHz( ) {
				using namespace core;
				radio_write_single_reg( ADDR, 0x00 ); //Device Address
				radio_write_single_reg( AGCCTRL0, 0x91 ); //AGC Control
				radio_write_single_reg( AGCCTRL1, 0x00 ); //AGC Control
				radio_write_single_reg( AGCCTRL2, 0x03 ); //AGC Control
				radio_write_single_reg( CHANNR, 0x02 ); //Channel Number
				radio_write_single_reg( DEVIATN, 0x15 ); //Modem Deviation Setting
				radio_write_single_reg( FIFOTHR, 0x47 ); //RX FIFO and TX FIFO Thresholds
				radio_write_single_reg( FOCCFG, 0x17 ); //Frequency Offset Compensation Configuration
				radio_write_single_reg( FREND0, 0x11 ); //Front End TX Configuration
				radio_write_single_reg( FREND1, 0x56 ); //Front End RX Configuration
														//radio_write_single_reg( FREQ0, 0x19 ); //Frequency Control Word, Low Byte
				radio_write_single_reg( FREQ0, 0x00 ); //Frequency Control Word, Low Byte
				radio_write_single_reg( FREQ1, 0x40 ); //Frequency Control Word, Middle Byte
				radio_write_single_reg( FREQ2, 0x23 ); //Frequency Control Word, High Byte
				radio_write_single_reg( FSCAL0, 0x1F ); //Frequency Synthesizer Calibration
				radio_write_single_reg( FSCAL1, 0x00 ); //Frequency Synthesizer Calibration
				radio_write_single_reg( FSCAL2, 0x2A ); //Frequency Synthesizer Calibration
				radio_write_single_reg( FSCAL3, 0xE9 ); //Frequency Synthesizer Calibration
				radio_write_single_reg( FSCTRL0, 0x00 ); //Frequency Synthesizer Control
				radio_write_single_reg( FSCTRL1, 0x06 ); //Frequency Synthesizer Control
				radio_write_single_reg( MCSM0, 0x10 ); //Main Radio Control State Machine Configuration
				radio_write_single_reg( MCSM1, 0x30 ); //Main Radio Control State Machine Configuration
				radio_write_single_reg( MCSM2, 0x07 ); //Main Radio Control State Machine Configuration
				radio_write_single_reg( MDMCFG0, 0xC5 ); //Modem Configuration
				radio_write_single_reg( MDMCFG1, 0x11 ); //Modem Configuration
				radio_write_single_reg( MDMCFG2, 0x32 ); //Modem Configuration
				radio_write_single_reg( MDMCFG3, 0x4A ); //Modem Configuration
				radio_write_single_reg( MDMCFG4, 0xC9 ); //Modem Configuration
				radio_write_single_reg( PKTCTRL0, 0x01 ); //Packet Automation Control
				radio_write_single_reg( PKTCTRL1, 0x04 ); //Packet Automation Control
				radio_write_single_reg( PKTLEN, 0xFF ); //Packet Length
				radio_write_single_reg( SYNC0, 0x00 ); //Sync Word, Low Byte
				radio_write_single_reg( SYNC1, 0xFF ); //Sync Word, High Byte
				radio_write_single_reg( TEST0, 0x09 ); //Various Test Settings
				radio_write_single_reg( TEST1, 0x35 ); //Various Test Settings
				radio_write_single_reg( TEST2, 0x81 ); //Various Test Settings

				{
					uint8_t const pa_values[8] = { 0x00, 0x00, 0x52, 0x00, 0x00, 0x00, 0x00, 0x00 };
					radio_write_burst_pa_table( pa_values, 8 );
				}
			}
		} // namespace medtronic
	} // namespace radio
}	// namespace daw