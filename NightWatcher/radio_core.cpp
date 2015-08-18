#include "radio_core.h"
#include <stdint.h>

namespace daw {
	namespace radio {
		namespace core {
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
				uint16_t mode = 0; // save actual state of FLL loop control
				auto const globalInterruptState = __get_SR_register( ) & SCG0;
				__bic_SR_register( SCG0 ); // Disable FLL loop control

				d = ratio;
				dco_div_bits = FLLD__2; // Have at least a divider of 2
				if( fsystem > 16000 ) {
					d >>= 1;
					mode = 1;
				} else {
					fsystem <<= 1; // fsystem = fsystem * 2
				}

				while( d > 512 ) {
					dco_div_bits = dco_div_bits + FLLD0; // set next higher div level
					d >>= 1;
				}

				UCSCTL0 = 0x000; // Set DCO to lowest Tap

				UCSCTL2 &= ~(0x3FF); // Reset FN bits
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
					UCSCTL7 &= ~(DCOFFG + XT1LFOFFG + XT1HFOFFG + XT2OFFG); // Clear OSC flaut Flags
					SFRIFG1 &= ~OFIFG; // Clear OFIFG fault flag
				}

				if( mode == 1 ) {                          		  // fsystem > 16000
					SELECT_MCLK_SMCLK( SELM__DCOCLK + SELS__DCOCLK ); // select DCOCLK
				} else {
					SELECT_MCLK_SMCLK( SELM__DCOCLKDIV + SELS__DCOCLKDIV ); // select DCODIVCLK
				}
				__bis_SR_register( globalInterruptState ); // restore previous state
			} // End of fll_init()

			uint16_t set_vcore_up( uint16_t const & level ) {
				uint16_t PMMRIE_backup;
				uint16_t SVSMHCTL_backup; // Open PMM registers for write access
				PMMCTL0_H = 0xA5; // Disable dedicated Interrupts to prevent that needed flags will be cleared
				PMMRIE_backup = PMMRIE;
				PMMRIE &= ~(SVSMHDLYIE | SVSMLDLYIE | SVMLVLRIE | SVMHVLRIE | SVMHVLRPE); // Set SVM high side to new level and check if a VCore increase is possible
				SVSMHCTL_backup = SVSMHCTL;
				PMMIFG &= ~(SVMHIFG | SVSMHDLYIFG);
				SVSMHCTL = SVMHE | SVMHFP | (SVSMHRRL0 * level); // Wait until SVM high side is settled
				while( 0 == (PMMIFG & SVSMHDLYIFG) ) { /* spin */ }

				// Disable full-performance mode to save energy
				SVSMHCTL &= ~_HAL_PMM_SVSFP; // Check if a VCore increase is possible
				if( (PMMIFG & SVMHIFG) == SVMHIFG ) {			//-> Vcc is to low for a Vcore increase recover the previous settings
					PMMIFG &= ~SVSMHDLYIFG;
					SVSMHCTL = SVSMHCTL_backup; // Wait until SVM high side is settled
					while( (PMMIFG & SVSMHDLYIFG) == 0 ) { /* spin */ }

					// Clear all Flags
					PMMIFG &= ~(SVMHVLRIFG | SVMHIFG | SVSMHDLYIFG | SVMLVLRIFG | SVMLIFG | SVSMLDLYIFG); // backup PMM-Interrupt-Register
					PMMRIE = PMMRIE_backup; // Lock PMM registers for write access
					PMMCTL0_H = 0x00;
					return PMM_STATUS_ERROR; // return: voltage not set
				}

				// Set also SVS high side to new level -> Vcc is high enough for a Vcore increase
				SVSMHCTL |= SVSHE | (SVSHRVL0 * level); // Set SVM low side to new level
				SVSMLCTL = SVMLE | SVMLFP | (SVSMLRRL0 * level); // Wait until SVM low side is settled
				while( (PMMIFG & SVSMLDLYIFG) == 0 ) { /* spin */ }

				// Clear already set flags
				PMMIFG &= ~(SVMLVLRIFG | SVMLIFG); // Set VCore to new level
				PMMCTL0_L = PMMCOREV0 * level; // Wait until new level reached
				if( PMMIFG & SVMLIFG ) {
					while( 0 == (PMMIFG & SVMLVLRIFG) ) { /* spin */ }
				}

				// Set also SVS/SVM low side to new level
				PMMIFG &= ~SVSMLDLYIFG;
				SVSMLCTL |= SVSLE | (SVSLRVL0 * level); // wait for low side delay flags
				while( 0 == (PMMIFG & SVSMLDLYIFG) ) { /* spin */ }

				// Disable SVS/SVM Low
				// Disable full-performance mode to save energy
				SVSMLCTL &= ~(_HAL_PMM_DISABLE_SVSL_ + _HAL_PMM_DISABLE_SVML_ + _HAL_PMM_SVSFP); // Clear all Flags
				PMMIFG &= ~(SVMHVLRIFG | SVMHIFG | SVSMHDLYIFG | SVMLVLRIFG | SVMLIFG | SVSMLDLYIFG); // backup PMM-Interrupt-Register
				PMMRIE = PMMRIE_backup; // Lock PMM registers for write access
				PMMCTL0_H = 0x00;
				return PMM_STATUS_OK; // return: OK
			}

			//****************************************************************************//
			// Set VCore down (Independent from the enabled Interrupts in PMMRIE)
			//****************************************************************************//
			uint16_t set_vcore_down( uint16_t const & level ) {
				PMMCTL0_H = 0xA5; // Disable dedicated Interrupts to prevent that needed flags will be cleared
				auto PMMRIE_backup = PMMRIE; // Open PMM registers for write access
				PMMRIE &= ~(SVSMHDLYIE | SVSMLDLYIE | SVMLVLRIE | SVMHVLRIE | SVMHVLRPE); // Set SVM high side and SVM low side to new level
				PMMIFG &= ~(SVMHIFG | SVSMHDLYIFG | SVMLIFG | SVSMLDLYIFG);
				SVSMHCTL = SVMHE | SVMHFP | (SVSMHRRL0 * level);
				SVSMLCTL = SVMLE | SVMLFP | (SVSMLRRL0 * level); // Wait until SVM high side and SVM low side is settled
				while( 0 == (PMMIFG & SVSMHDLYIFG) || 0 == (PMMIFG & SVSMLDLYIFG) ) { /* spin */ }

				// Set VCore to new level
				PMMCTL0_L = PMMCOREV0 * level; // Set also SVS high side and SVS low side to new level
				PMMIFG &= ~(SVSHIFG | SVSMHDLYIFG | SVSLIFG | SVSMLDLYIFG);
				SVSMHCTL |= SVSHE | SVSHFP | (SVSHRVL0 * level);
				SVSMLCTL |= SVSLE | SVSLFP | (SVSLRVL0 * level); // Wait until SVS high side and SVS low side is settled
				while( 0 == (PMMIFG & SVSMHDLYIFG) || 0 == (PMMIFG & SVSMLDLYIFG) ) { /* spin */ }

				// Disable full-performance mode to save energy
				SVSMHCTL &= ~_HAL_PMM_SVSFP; // Disable SVS/SVM Low
											 // Disable full-performance mode to save energy
				SVSMLCTL &= ~(_HAL_PMM_DISABLE_SVSL_ + _HAL_PMM_DISABLE_SVML_ + _HAL_PMM_SVSFP); // Clear all Flags
				PMMIFG &= ~(SVMHVLRIFG | SVMHIFG | SVSMHDLYIFG | SVMLVLRIFG | SVMLIFG | SVSMLDLYIFG); // backup PMM-Interrupt-Register
				PMMRIE = PMMRIE_backup; // Lock PMM registers for write access
				PMMCTL0_H = 0x00;

				if( (PMMIFG & SVMHIFG) == SVMHIFG ) {
					return PMM_STATUS_ERROR; // High side is still to low for the adjusted VCore Level
				}

				return PMM_STATUS_OK; // Return: OK
			}

			//****************************************************************************//
			// Set VCore
			//****************************************************************************//
			uint16_t set_vcore( uint16_t level ) {
				uint16_t status = 0;

				level &= PMMCOREV_3; // Set Mask for Max. level
				auto actlevel = (PMMCTL0 & PMMCOREV_3); // Get actual VCore

				while( (level != actlevel && 0 == status) || level < actlevel ) {	// step by step increase or decrease
					if( level > actlevel ) {
						status = set_vcore_up( ++actlevel );
					} else {
						status = set_vcore_down( --actlevel );
					}
				}
				return status;
			}

			uint8_t strobe( uint8_t const cmd ) {
				uint8_t status_byte = 0;
				if( 0xBD == cmd || (cmd >= RF_SRES && cmd <= RF_SNOP) ) {
					// Clear the Status read flag
					RF1AIFCTL1 &= ~(RFSTATIFG); // Wait for radio to be ready for next instruction
					while( !(RF1AIFCTL1 & RFINSTRIFG) ) { /* spin */ }

					if( (cmd > RF_SRES) && (cmd < RF_SNOP) ) {
						auto gdo_state = radio_read_single_reg( IOCFG2 ); // buffer IOCFG2 state
						radio_write_single_reg( IOCFG2, 0x29 ); // chip-ready to GDO2

						RF1AINSTRB = cmd;
						if( 0x04 == (RF1AIN & 0x04) ) {	// chip at sleep mode
							if( !((cmd == RF_SXOFF) || (cmd == RF_SPWD) || (cmd == RF_SWOR)) ) {
								while( 0x04 == (RF1AIN & 0x04) ) { /* spin */ }	// chip-ready ?

								__delay_cycles( 850 ); // Delay for ~810usec at 1.05MHz CPU clock, see erratum RF1A7
							}
						}
						radio_write_single_reg( IOCFG2, gdo_state ); // restore IOCFG2 setting

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
				RF1AINSTRB = addr | RF_SNGLREGWR; // Send address + Instruction
				RF1ADINB = value; // Write data in
				__no_operation( );
			}

			uint8_t radio_read_single_reg( uint8_t const & addr ) {
				if( (addr <= 0x2E) || (addr == 0x3E) ) {
					RF1AINSTR1B = (addr | RF_SNGLREGRD); // Send address + Instruction + 1 dummy byte (auto-read)
				} else {
					RF1AINSTR1B = (addr | RF_STATREGRD); // Send address + Instruction + 1 dummy byte (auto-read)
				}

				while( !(RF1AIFCTL1 & RFDOUTIFG) ) { /* spin */ }

				auto data_out = RF1ADOUTB; // Read data and clears the RFDOUTIFG
				return data_out;
			}

			void radio_write_single_pa_table( uint8_t const & value ) {
				while( !(RF1AIFCTL1 & RFINSTRIFG) ) { /* spin */ }

				RF1AINSTRW = 0x3E00 + value; // PA Table single write

				while( !(RF1AIFCTL1 & RFINSTRIFG) ) { /* spin */ }
				RF1AINSTRB = RF_SNOP; // reset PA_Table pointer
			}

			void radio_write_burst_pa_table( uint8_t const * const buffer, size_t const & count ) {
				while( !(RF1AIFCTL1 & RFINSTRIFG) ) { /* spin */ }

				volatile size_t i = 0;
				RF1AINSTRW = 0x7E00 + buffer[i]; // PA Table burst write

				daw::for_each( buffer + i, buffer + count, []( uint8_t const & value ) {
					RF1ADINB = value; // Send data
					while( !(RFDINIFG & RF1AIFCTL1) ) { /* spin */ }	// Wait for TX to finish
				} );
				i = RF1ADOUTB; // Reset RFDOUTIFG flag which contains status byte

				while( !(RF1AIFCTL1 & RFINSTRIFG) ) { /* spin */ }
				RF1AINSTRB = RF_SNOP; // reset PA Table pointer
			}

			void reset_core( ) {
				strobe( RF_SRES ); // Reset the Radio Core
				strobe( RF_SNOP ); // Reset Radio Pointer
			}
		}	// namespace core
	} // namespace radio
} // namespace daw