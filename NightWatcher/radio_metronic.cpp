#include "radio_medtronic.h"
#include "radio_core.h"

//std::array<uint8_t, 54> const radio_symbol_table = { 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 11, 16, 13, 14, 16, 16, 16, 16, 16, 16, 0, 7, 16, 16, 9, 8, 16, 15, 16, 16, 16, 16, 16, 16, 3, 16, 5, 6, 16, 16, 16, 10, 16, 12, 16, 16, 16, 16, 1, 2, 16, 4 };

void radio_setup_916MHz( ) {
	daw::radio::radio_write_single_reg( AGCCTRL0, 0x91 );
	daw::radio::radio_write_single_reg( AGCCTRL1, 0x40 );
	daw::radio::radio_write_single_reg( AGCCTRL2, 0x03 );
	daw::radio::radio_write_single_reg( CHANNR, 0x02 );       //Channel Number
	daw::radio::radio_write_single_reg( DEVIATN, 0x15 );      //Modem Deviation Setting
	daw::radio::radio_write_single_reg( FOCCFG, 0x17 );       //Frequency Offset Compensation Configuration
	daw::radio::radio_write_single_reg( FREND0, 0x11 );       //Front End TX Configuration	// DAW May be 0x12 from mmcommander
	daw::radio::radio_write_single_reg( FREND1, 0x56 );
	daw::radio::radio_write_single_reg( FREQ0, 0x00 );        //Frequency Control Word, Low Byte
	daw::radio::radio_write_single_reg( FREQ1, 0x40 );        //Frequency Control Word, Middle Byte
	daw::radio::radio_write_single_reg( FREQ2, 0x23 );        //Frequency Control Word, High Byte
	daw::radio::radio_write_single_reg( FSCAL0, 0x1F );       //Frequency Synthesizer Calibration
	daw::radio::radio_write_single_reg( FSCAL1, 0x00 );       //Frequency Synthesizer Calibration
	daw::radio::radio_write_single_reg( FSCAL2, 0x2A );       //Frequency Synthesizer Calibration
	daw::radio::radio_write_single_reg( FSCAL3, 0xE9 );       //Frequency Synthesizer Calibration
	daw::radio::radio_write_single_reg( FSCTRL0, 0x0 );      //Frequency Synthesizer Control
	daw::radio::radio_write_single_reg( FSCTRL1, 0x06 );      //Frequency Synthesizer Control
	daw::radio::radio_write_single_reg( IOCFG0, 0x06 );       //GDO0 Output Configuration
	daw::radio::radio_write_single_reg( LQI, 0xFF );          //Demodulator Estimate for Link Quality
	daw::radio::radio_write_single_reg( MARCSTATE, 0x01 );    //Main Radio Control State Machine State
	daw::radio::radio_write_single_reg( MCSM0, 0x10 );        //Main Radio Control State Machine Configuration
	daw::radio::radio_write_single_reg( MDMCFG0, 0xC5 );      //Modem Configuration
	daw::radio::radio_write_single_reg( MDMCFG1, 0x11 );      //Modem Configuration
	daw::radio::radio_write_single_reg( MDMCFG2, 0x33 );      //Modem Configuration
	daw::radio::radio_write_single_reg( MDMCFG3, 0x4A );      //Modem Configuration
	daw::radio::radio_write_single_reg( MDMCFG4, 0x69 );      //Modem Configuration
	daw::radio::radio_write_single_reg( PKTCTRL0, 0x00 );     //Packet Automation Control
	daw::radio::radio_write_single_reg( PKTLEN, 0x10 );       //Packet Length
	daw::radio::radio_write_single_reg( PKTSTATUS, 0xA0 );    //Current GDOx Status and Packet Status
	daw::radio::radio_write_single_reg( TEST2, 0x88 );
	daw::radio::radio_write_single_reg( TEST1, 0x31 );

// 	RF1ADINW = 0xC002;   //Radio word data in register
// 	RF1AIE = 0x200;      //Radio core interrupt enable register
// 	RF1AIES = 0x200;     //Radio core interrupt edge select register
// 	RF1AIFCTL1 = 0x74;   //Radio interface control register 1
// 	RF1AIFERR = 0x01;    //Radio interface error flag register
// 	RF1AIFERRV = 0x02;   //Radio interface error vector word register
// 	RF1AIFG = 0x3E19;    //Radio core interrupt flag register
// 	RF1AIN = 0xC00;      //Radio core signal input register
// 	RF1AINSTR1W = 0xC000;//Radio instruction word register with 1-byte auto-r..
// 	RF1AINSTR2W = 0xC000;//Radio instruction word register with 2-byte auto-r..
// 	RF1AINSTRW = 0xC002; //Radio instruction word register
// 	RF1AIV = 0x14;       //Radio core interrupt vector word register
	daw::radio::radio_write_single_reg( RSSI, 0xFC );         //Received Signal Strength Indication
	daw::radio::radio_write_single_reg( SYNC0, 0x00 );        //Sync Word, Low Byte
	daw::radio::radio_write_single_reg( SYNC1, 0xFF );        //Sync Word, High Byte
	daw::radio::radio_write_single_reg( TEST0, 0x09 );        //Various Test Settings
	daw::radio::radio_write_single_reg( VCO_VC_DAC, 0xFC );   //Current Setting from PLL Calibration Module
// 	daw::radio::radio_write_single_reg( WORCTRL, 0xF8 );      //Wake On Radio Control
// 	daw::radio::radio_write_single_reg( WOREVT0, 0x6B );      //Low Byte Event0 Timeout
// 	daw::radio::radio_write_single_reg( WOREVT1, 0x87 );      //High Byte Event0 Timeout

	{
		std::array<uint8_t, 8> const pa_values = { 0x00,0x00,0x52,0x00,0x00,0x00,0x00,0x00, };
		daw::radio::radio_write_burst_pa_table( pa_values.data( ), pa_values.size( ) );
	}
}