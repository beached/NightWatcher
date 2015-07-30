#include "radio_core.h"
#include <cstdint>
#include <functional>

namespace daw {
	namespace radio {
#ifndef st
#define st(x)      do { x } while (__LINE__ == -1)
#endif	// st

		// Select source for MCLK and SMCLK e.g. SELECT_MCLK_SMCLK(SELM__DCOCLK + SELS__DCOCLK)
#ifndef SELECT_MCLK_SMCLK
#define SELECT_MCLK_SMCLK(sources) st(UCSCTL4 = (UCSCTL4 & ~(SELM_7 + SELS_7)) | (sources);)
#endif	//SELECT_MCLK_SMCLK

//====================================================================
/**
* Initializes FLL of the UCS
*
* \param fsystem  required system frequency (MCLK) in kHz
* \param ratio    ratio between fsystem and FLLREFCLK
*/
		void init_fll( uint16_t fsystem, uint16_t ratio ) {
			uint16_t d, dco_div_bits;
			uint16_t mode = 0;

			// save actual state of FLL loop control
			uint16_t globalInterruptState = __get_SR_register( ) & SCG0;
			__bic_SR_register( SCG0 );      // Disable FLL loop control

			d = ratio;
			dco_div_bits = FLLD__2;        // Have at least a divider of 2
			if( fsystem > 16000 ) {
				d >>= 1;
				mode = 1;
			} else {
				fsystem <<= 1;               // fsystem = fsystem * 2
			}

			while( d > 512 ) {
				dco_div_bits = dco_div_bits + FLLD0;  // set next higher div level
				d >>= 1;
			}

			UCSCTL0 = 0x000;               // Set DCO to lowest Tap

			UCSCTL2 &= ~(0x3FF);           // Reset FN bits
			UCSCTL2 = dco_div_bits | (d - 1);

			if( fsystem <= 630 ) {    //           fsystem < 0.63MHz
				UCSCTL1 = DCORSEL_0;
			} else if( fsystem < 1250 ) { // 0.63MHz < fsystem < 1.25MHz
				UCSCTL1 = DCORSEL_1;
			} else if( fsystem < 2500 ) {    // 1.25MHz < fsystem <  2.5MHz
				UCSCTL1 = DCORSEL_2;
			} else if( fsystem < 5000 ) {    // 2.5MHz  < fsystem <    5MHz
				UCSCTL1 = DCORSEL_3;
			} else if( fsystem < 10000 ) {   // 5MHz    < fsystem <   10MHz
				UCSCTL1 = DCORSEL_4;
			} else if( fsystem < 20000 ) {   // 10MHz   < fsystem <   20MHz
				UCSCTL1 = DCORSEL_5;
			} else if( fsystem < 40000 ) {    // 20MHz   < fsystem <   40MHz
				UCSCTL1 = DCORSEL_6;
			} else {
				UCSCTL1 = DCORSEL_7;
			}
			/* if CC430EM does not use LFXT1 */
			while( UCSCTL7 & (DCOFFG + XT1HFOFFG + XT2OFFG) ) {                               // check OFIFG fault flag
				UCSCTL7 &= ~(DCOFFG + XT1LFOFFG + XT1HFOFFG + XT2OFFG);     // Clear OSC flaut Flags
				SFRIFG1 &= ~OFIFG;                                    // Clear OFIFG fault flag
			}

			if( mode == 1 ) {                          		  // fsystem > 16000
				SELECT_MCLK_SMCLK( SELM__DCOCLK + SELS__DCOCLK );       // select DCOCLK
			} else {
				SELECT_MCLK_SMCLK( SELM__DCOCLKDIV + SELS__DCOCLKDIV ); // selcet DCODIVCLK
			}
			__bis_SR_register( globalInterruptState );                // restore previous state
		} // End of fll_init()

		uint8_t strobe( uint8_t const cmd ) {
			uint8_t status_byte = 0;
			if( (cmd == 0xBD) || ((cmd >= RF_SRES) && (cmd <= RF_SNOP)) ) {
				// Clear the Status read flag
				RF1AIFCTL1 &= ~(RFSTATIFG);

				// Wait for radio to be ready for next instruction
				while( !(RF1AIFCTL1 & RFINSTRIFG) ) { /* spin */ }

				if( (cmd > RF_SRES) && (cmd < RF_SNOP) ) {
					uint8_t gdo_state = radio_read_single_reg( IOCFG2 );    // buffer IOCFG2 state
					radio_write_single_reg( IOCFG2, 0x29 );         // chip-ready to GDO2

					RF1AINSTRB = cmd;
					if( (RF1AIN & 0x04) == 0x04 ) {	// chip at sleep mode
						if( !((cmd == RF_SXOFF) || (cmd == RF_SPWD) || (cmd == RF_SWOR)) ) {
							while( (RF1AIN & 0x04) == 0x04 ) { /* spin */ }	// chip-ready ?

							__delay_cycles( 850 );	// Delay for ~810usec at 1.05MHz CPU clock, see erratum RF1A7
						}
					}
					radio_write_single_reg( IOCFG2, gdo_state );    // restore IOCFG2 setting

					while( !(RF1AIFCTL1 & RFSTATIFG) ) { /* spin */ }
				} else {	                    // chip active mode (SRES)
					RF1AINSTRB = cmd;
				}
				status_byte = RF1ASTATB;
			}
			return status_byte;
		}

		void radio_write_single_reg( uint8_t const & addr, uint8_t const & value ) {
			while( !(RF1AIFCTL1 & RFINSTRIFG) ) { /* spin */ }	// Wait for the Radio to be ready for next instruction
			RF1AINSTRB = addr | RF_SNGLREGWR;	// Send address + Instruction
			RF1ADINB = value;	// Write data in
			__no_operation( );
		}

		uint8_t radio_read_single_reg( uint8_t const & addr ) {
			uint8_t data_out;

			// Check for valid configuration register address, 0x3E refers to PATABLE
			if( (addr <= 0x2E) || (addr == 0x3E) ) {
				RF1AINSTR1B = (addr | RF_SNGLREGRD);	// Send address + Instruction + 1 dummy byte (auto-read)
			} else {
				RF1AINSTR1B = (addr | RF_STATREGRD);	// Send address + Instruction + 1 dummy byte (auto-read)
			}

			while( !(RF1AIFCTL1 & RFDOUTIFG) ) { /* spin */ }

			data_out = RF1ADOUTB;	// Read data and clears the RFDOUTIFG
			return data_out;
		}

		void radio_write_single_pa_table( uint8_t const & value ) {
			while( !(RF1AIFCTL1 & RFINSTRIFG) ) { /* spin */ }

			RF1AINSTRW = 0x3E00 + value;	// PA Table single write

			while( !(RF1AIFCTL1 & RFINSTRIFG) ) { /* spin */ }
			RF1AINSTRB = RF_SNOP;	// reset PA_Table pointer
		}

		void radio_write_burst_pa_table( uint8_t const * const buffer, size_t const & count ) {
			volatile size_t i = 0;

			while( !(RF1AIFCTL1 & RFINSTRIFG) ) { /* spin */ }
			RF1AINSTRW = 0x7E00 + buffer[i];	// PA Table burst write

			for( i = 1; i < count; ++i ) {
				RF1ADINB = buffer[i];	// Send data
				while( !(RFDINIFG & RF1AIFCTL1) ) { /* spin */ }	// Wait for TX to finish
			}
			i = RF1ADOUTB;	// Reset RFDOUTIFG flag which contains status byte

			while( !(RF1AIFCTL1 & RFINSTRIFG) ) { /* spin */ }
			RF1AINSTRB = RF_SNOP;	// reset PA Table pointer
		}

		void reset_core( ) {
			strobe( RF_SRES );	// Reset the Radio Core
			strobe( RF_SNOP );	// Reset Radio Pointer
		}
	}
}