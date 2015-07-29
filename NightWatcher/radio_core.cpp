#include "radio_core.h"
#include <cstdint>
#include <functional>

namespace daw {
	namespace radio {
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