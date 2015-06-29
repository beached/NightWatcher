#include "radio.h"
#include <cstdint>
namespace {
	// *****************************************************************************
	// @fn          radio_write_single_reg
	// @brief       Write a single byte to a radio register
	// @param       uint8_t const & addr      Target radio register address
	// @param       uint8_t const & value     Value to be written
	// @return      none
	// *****************************************************************************
	void radio_write_single_reg( uint8_t const & addr, uint8_t const & value ) {
		while( !(RF1AIFCTL1 & RFINSTRIFG) );	// Wait for the Radio to be ready for next instruction
		RF1AINSTRB = addr | RF_SNGLREGWR;	// Send address + Instruction
		RF1ADINB = value;	// Write data in
		__no_operation( );
	}

	// *****************************************************************************
	// @fn          radio_read_single_reg
	// @brief       Read a single byte from the radio register
	// @param       uint8_t const & addr      Target radio register address
	// @return      uint8_t data_out  Value of byte that was read
	// *****************************************************************************
	uint8_t radio_read_single_reg( uint8_t const & addr ) {
		uint8_t data_out;

		// Check for valid configuration register address, 0x3E refers to PATABLE
		if( (addr <= 0x2E) || (addr == 0x3E) ) {
			RF1AINSTR1B = (addr | RF_SNGLREGRD);	// Send address + Instruction + 1 dummy byte (auto-read)
		} else {
			RF1AINSTR1B = (addr | RF_STATREGRD);	// Send address + Instruction + 1 dummy byte (auto-read)
		}

		while( !(RF1AIFCTL1 & RFDOUTIFG) );

		data_out = RF1ADOUTB;	// Read data and clears the RFDOUTIFG
		return data_out;
	}

	std::array<uint8_t, 54> const radio_symbol_table = { 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 11, 16, 13, 14, 16, 16, 16, 16, 16, 16, 0, 7, 16, 16, 9, 8, 16, 15, 16, 16, 16, 16, 16, 16, 3, 16, 5, 6, 16, 16, 16, 10, 16, 12, 16, 16, 16, 16, 1, 2, 16, 4 };

#define _HAL_PMM_DISABLE_SVML_
#define _HAL_PMM_DISABLE_SVSL_
#define _HAL_PMM_DISABLE_FULL_PERFORMANCE_

	//****************************************************************************//
#ifdef _HAL_PMM_DISABLE_SVML_
#define _HAL_PMM_SVMLE SVMLE
#else
#define _HAL_PMM_SVMLE 0
#endif
#ifdef _HAL_PMM_DISABLE_SVSL_
#define _HAL_PMM_SVSLE SVSLE
#else
#define _HAL_PMM_SVSLE 0
#endif
#ifdef _HAL_PMM_DISABLE_FULL_PERFORMANCE_
#define _HAL_PMM_SVSFP SVSLFP
#else
#define _HAL_PMM_SVSFP 0
#endif

#define PMM_STATUS_OK     0
#define PMM_STATUS_ERROR  1

	//****************************************************************************//
	// Set VCore Up
	//****************************************************************************//
	uint16_t set_vcore_up( uint8_t level ) {
		uint16_t PMMRIE_backup;
		uint16_t SVSMHCTL_backup;

		// Open PMM registers for write access
		PMMCTL0_H = 0xA5;

		// Disable dedicated Interrupts to prevent that needed flags will be cleared
		PMMRIE_backup = PMMRIE;
		PMMRIE &= ~(SVSMHDLYIE | SVSMLDLYIE | SVMLVLRIE | SVMHVLRIE | SVMHVLRPE);

		// Set SVM highside to new level and check if a VCore increase is possible
		SVSMHCTL_backup = SVSMHCTL;
		PMMIFG &= ~(SVMHIFG | SVSMHDLYIFG);
		SVSMHCTL = SVMHE | SVMHFP | (SVSMHRRL0 * level);

		// Wait until SVM highside is settled
		while( (PMMIFG & SVSMHDLYIFG) == 0 );

		// Disable full-performance mode to save energy
		SVSMHCTL &= ~_HAL_PMM_SVSFP;
		// Check if a VCore increase is possible
		if( (PMMIFG & SVMHIFG) == SVMHIFG ) {			//-> Vcc is to low for a Vcore increase recover the previous settings
			PMMIFG &= ~SVSMHDLYIFG;
			SVSMHCTL = SVSMHCTL_backup;

			// Wait until SVM highside is settled
			while( (PMMIFG & SVSMHDLYIFG) == 0 );

			// Clear all Flags
			PMMIFG &= ~(SVMHVLRIFG | SVMHIFG | SVSMHDLYIFG | SVMLVLRIFG | SVMLIFG | SVSMLDLYIFG);

			// backup PMM-Interrupt-Register
			PMMRIE = PMMRIE_backup;

			// Lock PMM registers for write access
			PMMCTL0_H = 0x00;
			return PMM_STATUS_ERROR;	// return: voltage not set
		}

		// Set also SVS highside to new level -> Vcc is high enough for a Vcore increase
		SVSMHCTL |= SVSHE | (SVSHRVL0 * level);

		// Set SVM low side to new level
		SVSMLCTL = SVMLE | SVMLFP | (SVSMLRRL0 * level);

		// Wait until SVM low side is settled
		while( (PMMIFG & SVSMLDLYIFG) == 0 );

		// Clear already set flags
		PMMIFG &= ~(SVMLVLRIFG | SVMLIFG);

		// Set VCore to new level
		PMMCTL0_L = PMMCOREV0 * level;

		// Wait until new level reached
		if( PMMIFG & SVMLIFG ) {
			while( (PMMIFG & SVMLVLRIFG) == 0 );
		}

		// Set also SVS/SVM low side to new level
		PMMIFG &= ~SVSMLDLYIFG;
		SVSMLCTL |= SVSLE | (SVSLRVL0 * level);

		// wait for lowside delay flags
		while( (PMMIFG & SVSMLDLYIFG) == 0 );

		// Disable SVS/SVM Low
		// Disable full-performance mode to save energy
		SVSMLCTL &= ~(_HAL_PMM_DISABLE_SVSL_ + _HAL_PMM_DISABLE_SVML_ + _HAL_PMM_SVSFP);

		// Clear all Flags
		PMMIFG &= ~(SVMHVLRIFG | SVMHIFG | SVSMHDLYIFG | SVMLVLRIFG | SVMLIFG | SVSMLDLYIFG);
		// backup PMM-Interrupt-Register
		PMMRIE = PMMRIE_backup;

		// Lock PMM registers for write access
		PMMCTL0_H = 0x00;
		return PMM_STATUS_OK;	// return: OK
	}

	//****************************************************************************//
	// Set VCore down (Independent from the enabled Interrupts in PMMRIE)
	//****************************************************************************//
	uint16_t set_vcore_down( uint8_t level ) {
		uint16_t PMMRIE_backup;

		// Open PMM registers for write access
		PMMCTL0_H = 0xA5;

		// Disable dedicated Interrupts to prevent that needed flags will be cleared
		PMMRIE_backup = PMMRIE;
		PMMRIE &= ~(SVSMHDLYIE | SVSMLDLYIE | SVMLVLRIE | SVMHVLRIE | SVMHVLRPE);

		// Set SVM high side and SVM low side to new level
		PMMIFG &= ~(SVMHIFG | SVSMHDLYIFG | SVMLIFG | SVSMLDLYIFG);
		SVSMHCTL = SVMHE | SVMHFP | (SVSMHRRL0 * level);
		SVSMLCTL = SVMLE | SVMLFP | (SVSMLRRL0 * level);

		// Wait until SVM high side and SVM low side is settled
		while( (PMMIFG & SVSMHDLYIFG) == 0 || (PMMIFG & SVSMLDLYIFG) == 0 );

		// Set VCore to new level
		PMMCTL0_L = PMMCOREV0 * level;

		// Set also SVS highside and SVS low side to new level
		PMMIFG &= ~(SVSHIFG | SVSMHDLYIFG | SVSLIFG | SVSMLDLYIFG);
		SVSMHCTL |= SVSHE | SVSHFP | (SVSHRVL0 * level);
		SVSMLCTL |= SVSLE | SVSLFP | (SVSLRVL0 * level);

		// Wait until SVS high side and SVS low side is settled
		while( (PMMIFG & SVSMHDLYIFG) == 0 || (PMMIFG & SVSMLDLYIFG) == 0 );

		// Disable full-performance mode to save energy
		SVSMHCTL &= ~_HAL_PMM_SVSFP;

		// Disable SVS/SVM Low
		// Disable full-performance mode to save energy
		SVSMLCTL &= ~(_HAL_PMM_DISABLE_SVSL_ + _HAL_PMM_DISABLE_SVML_ + _HAL_PMM_SVSFP);

		// Clear all Flags
		PMMIFG &= ~(SVMHVLRIFG | SVMHIFG | SVSMHDLYIFG | SVMLVLRIFG | SVMLIFG | SVSMLDLYIFG);

		// backup PMM-Interrupt-Register
		PMMRIE = PMMRIE_backup;

		// Lock PMM registers for write access
		PMMCTL0_H = 0x00;

		if( (PMMIFG & SVMHIFG) == SVMHIFG ) {
			return PMM_STATUS_ERROR;	// Highside is still to low for the adjusted VCore Level
		} else {
			return PMM_STATUS_OK;	// Return: OK
		}
	}

	//****************************************************************************//
	// Set VCore
	//****************************************************************************//
	uint16_t set_vcore( uint16_t level ) {
		unsigned int status = 0;

		level &= PMMCOREV_3;	// Set Mask for Max. level
		uint16_t actlevel = (PMMCTL0 & PMMCOREV_3);	// Get actual VCore

		while( ((level != actlevel) && (status == 0)) || (level < actlevel) ) {	// step by step increase or decrease
			if( level > actlevel ) {
				status = set_vcore_up( ++actlevel );
			} else {
				status = set_vcore_down( --actlevel );
			}
		}
		return status;
	}

	// *****************************************************************************
	// @fn          WritePATable
	// @brief       Write data to power table
	// @param       uint8_t const & value		Value to write
	// @return      none
	// *****************************************************************************
	void radio_write_single_pa_table( uint8_t const & value ) {
		while( !(RF1AIFCTL1 & RFINSTRIFG) );

		RF1AINSTRW = 0x3E00 + value;	// PA Table single write

		while( !(RF1AIFCTL1 & RFINSTRIFG) );
		RF1AINSTRB = RF_SNOP;	// reset PA_Table pointer
	}

	// *****************************************************************************
	// @fn          WritePATable
	// @brief       Write to multiple locations in power table
	// @param       unsigned char *buffer	Pointer to the table of values to be written
	// @param       unsigned char count	Number of values to be written
	// @return      none
	// *****************************************************************************
	void radio_write_burst_pa_table( uint8_t const * const buffer, size_t const count ) {
		volatile size_t i = 0;

		while( !(RF1AIFCTL1 & RFINSTRIFG) );
		RF1AINSTRW = 0x7E00 + buffer[i];	// PA Table burst write

		for( i = 1; i < count; ++i ) {
			RF1ADINB = buffer[i];	// Send data
			while( !(RFDINIFG & RF1AIFCTL1) );	// Wait for TX to finish
		}
		i = RF1ADOUTB;	// Reset RFDOUTIFG flag which contains status byte

		while( !(RF1AIFCTL1 & RFINSTRIFG) );
		RF1AINSTRB = RF_SNOP;	// reset PA Table pointer
	}

	// *****************************************************************************
	// @fn          WriteRfSettings
	// @brief       Write the minimum set of RF configuration register settings
	// @param       RF_SETTINGS *pRfSettings  Pointer to the structure that holds the rf settings
	// @return      none
	// *****************************************************************************
	void radio_setup_916MHz( ) {
		radio_write_single_reg( SYNC1, 0xFF ); // sync word, high byte
		radio_write_single_reg( SYNC0, 0x00 ); // sync word, low byte
		radio_write_single_reg( PKTLEN, 0xFF ); // packet length
		radio_write_single_reg( PKTCTRL1, 0x00 ); // packet automation control
		radio_write_single_reg( PKTCTRL0, 0x00 ); // packet automation control
		radio_write_single_reg( ADDR, 0x00 );
		radio_write_single_reg( CHANNR, 0x02 ); // channel number
		radio_write_single_reg( FSCTRL1, 0x06 ); // frequency synthesizer control
		radio_write_single_reg( FSCTRL0, 0x00 );
		radio_write_single_reg( FREQ2, 0x23 ); // frequency control word, high byte
		radio_write_single_reg( FREQ1, 0x40 ); // frequency control word, middle byte
		radio_write_single_reg( FREQ0, 0x00 ); // frequency control word, low byte
		radio_write_single_reg( MDMCFG4, 0x69 ); // modem configuration
		radio_write_single_reg( MDMCFG3, 0x4A ); // modem configuration
		radio_write_single_reg( MDMCFG2, 0x33 ); // modem configuration
		radio_write_single_reg( MDMCFG1, 0x11 ); // modem configuration
		radio_write_single_reg( MDMCFG0, 0xC5 ); // modem configuration
		radio_write_single_reg( DEVIATN, 0x15 ); // modem deviation setting
		radio_write_single_reg( MCSM2, 0x07 );
		radio_write_single_reg( MCSM1, 0x30 );
		radio_write_single_reg( MCSM0, 0x18 ); // main radio control state machine configuration
		radio_write_single_reg( FOCCFG, 0x17 ); // frequency offset compensation configuration
		radio_write_single_reg( BSCFG, 0x6C );
		radio_write_single_reg( FREND1, 0x56 ); // front end tx configuration
		radio_write_single_reg( FREND0, 0x11 ); // front end tx configuration
		radio_write_single_reg( FSCAL3, 0xE9 ); // frequency synthesizer calibration
		radio_write_single_reg( FSCAL2, 0x2A ); // frequency synthesizer calibration
		radio_write_single_reg( FSCAL1, 0x00 ); // frequency synthesizer calibration
		radio_write_single_reg( FSCAL0, 0x1F ); // frequency synthesizer calibration
		radio_write_single_reg( TEST1, 0x31 ); // various test settings
		radio_write_single_reg( TEST0, 0x09 ); // various test settings
		{
			std::array<uint8_t, 8> const pa_values = { 0x00,0x8e,0x00,0x00,0x00,0x00,0x00,0x00, };
			radio_write_burst_pa_table( pa_values.data( ), pa_values.size( ) );
		}
		// 		radio_write_single_reg( PA_TABLE0, 0x00 ); // needs to be explicitly set!
		// 		radio_write_single_reg( PA_TABLE1, 0xC0 ); // pa power setting 10 dBm
	}

	void configure_radio( ) { }
}	// namespace anonymous

Radio::Radio( ) : m_rx_buffer( ) {
	m_rx_buffer.fill( 0 );

	// Increase PMMCOREV level to 2 in order to avoid low voltage error
	// when the RF core is enabled
	set_vcore( 2 );

	// Set the High-Power Mode Request Enable bit so LPM3 can be entered
	// with active radio enabled
	PMMCTL0_H = 0xA5;
	PMMCTL0_L |= PMMHPMRE_L;
	PMMCTL0_H = 0x00;
}

void Radio::receive_data( ) { }

uint8_t Radio::strobe( uint8_t const cmd ) {
	uint8_t status_byte = 0;
	if( (cmd == 0xBD) || ((cmd >= RF_SRES) && (cmd <= RF_SNOP)) ) {
		// Clear the Status read flag
		RF1AIFCTL1 &= ~(RFSTATIFG);

		// Wait for radio to be ready for next instruction
		while( !(RF1AIFCTL1 & RFINSTRIFG) );

		if( (cmd > RF_SRES) && (cmd < RF_SNOP) ) {
			uint8_t gdo_state = radio_read_single_reg( IOCFG2 );    // buffer IOCFG2 state
			radio_write_single_reg( IOCFG2, 0x29 );         // chip-ready to GDO2

			RF1AINSTRB = cmd;
			if( (RF1AIN & 0x04) == 0x04 ) {	// chip at sleep mode
				if( !((cmd == RF_SXOFF) || (cmd == RF_SPWD) || (cmd == RF_SWOR)) ) {
					while( (RF1AIN & 0x04) == 0x04 );	// chip-ready ?

					__delay_cycles( 850 );	// Delay for ~810usec at 1.05MHz CPU clock, see erratum RF1A7
				}
			}
			radio_write_single_reg( IOCFG2, gdo_state );    // restore IOCFG2 setting

			while( !(RF1AIFCTL1 & RFSTATIFG) );
		} else {	                    // chip active mode (SRES)
			RF1AINSTRB = cmd;
		}
		status_byte = RF1ASTATB;
	}
	return status_byte;
}

void Radio::reset_core( ) {
	strobe( RF_SRES );	// Reset the Radio Core
	strobe( RF_SNOP );	// Reset Radio Pointer
}