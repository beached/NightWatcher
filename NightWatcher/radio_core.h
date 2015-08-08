#pragma once

#include <cc430f6137.h>
#include <stdint.h>
#include <cstring>
#include "buffer.h"

#define _HAL_PMM_DISABLE_SVML_
#define _HAL_PMM_SVMLE SVMLE
#define _HAL_PMM_DISABLE_SVSL_
#define _HAL_PMM_SVSLE SVSLE
#define _HAL_PMM_DISABLE_FULL_PERFORMANCE_
#define _HAL_PMM_SVSFP SVSLFP
#define PMM_STATUS_OK 0
#define PMM_STATUS_ERROR 1

namespace daw {
	namespace radio {
		extern void init_fll( uint16_t fsystem, uint16_t ratio );

		//****************************************************************************//
		// Set VCore Up
		//****************************************************************************//
		template<typename T>
		uint16_t set_vcore_up( T level ) {
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
			while( (PMMIFG & SVSMHDLYIFG) == 0 ) { /* spin */ }

			// Disable full-performance mode to save energy
			SVSMHCTL &= ~_HAL_PMM_SVSFP;
			// Check if a VCore increase is possible
			if( (PMMIFG & SVMHIFG) == SVMHIFG ) {			//-> Vcc is to low for a Vcore increase recover the previous settings
				PMMIFG &= ~SVSMHDLYIFG;
				SVSMHCTL = SVSMHCTL_backup;

				// Wait until SVM highside is settled
				while( (PMMIFG & SVSMHDLYIFG) == 0 ) { /* spin */ }

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
			while( (PMMIFG & SVSMLDLYIFG) == 0 ) { /* spin */ }

			// Clear already set flags
			PMMIFG &= ~(SVMLVLRIFG | SVMLIFG);

			// Set VCore to new level
			PMMCTL0_L = PMMCOREV0 * level;

			// Wait until new level reached
			if( PMMIFG & SVMLIFG ) {
				while( (PMMIFG & SVMLVLRIFG) == 0 ) { /* spin */ }
			}

			// Set also SVS/SVM low side to new level
			PMMIFG &= ~SVSMLDLYIFG;
			SVSMLCTL |= SVSLE | (SVSLRVL0 * level);

			// wait for lowside delay flags
			while( (PMMIFG & SVSMLDLYIFG) == 0 ) { /* spin */ }

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
		template<typename T>
		uint16_t set_vcore_down( T level ) {
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
			while( (PMMIFG & SVSMHDLYIFG) == 0 || (PMMIFG & SVSMLDLYIFG) == 0 ) { /* spin */ }

			// Set VCore to new level
			PMMCTL0_L = PMMCOREV0 * level;

			// Set also SVS highside and SVS low side to new level
			PMMIFG &= ~(SVSHIFG | SVSMHDLYIFG | SVSLIFG | SVSMLDLYIFG);
			SVSMHCTL |= SVSHE | SVSHFP | (SVSHRVL0 * level);
			SVSMLCTL |= SVSLE | SVSLFP | (SVSLRVL0 * level);

			// Wait until SVS high side and SVS low side is settled
			while( (PMMIFG & SVSMHDLYIFG) == 0 || (PMMIFG & SVSMLDLYIFG) == 0 ) { /* spin */ }

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
		template<typename T>
		uint16_t set_vcore( T level ) {
			uint16_t status = 0;

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

		template<typename SequenceContainer, typename ValueType>
		void sfill( SequenceContainer & container, ValueType const & value ) {
			//memset_s( container.data( ), container.size( ), static_cast<int>(value), container.size( ) );
			std::fill( container.begin( ), container.end( ), 0 );
		}

		template<typename T>
		T minimum( T const & n, T const & m ) {
			return m < n ? m : n;
		}

		extern uint8_t strobe( uint8_t const cmd );
		extern void radio_write_single_reg( uint8_t const & addr, uint8_t const & value );
		extern uint8_t radio_read_single_reg( uint8_t const & addr );

		template<typename ArryType>
		static void radio_read_burst_reg( uint8_t const & addr, ArryType & buffer, uint8_t const & count ) {
			while( !(RF1AIFCTL1 & RFINSTRIFG) ) { /* spin */ }

			RF1AINSTR1B = (addr | RF_REGRD);

			for( size_t i = 1; i < count; ++i ) {
				buffer[i - 1] = RF1ADOUT1B;
			}
			buffer[count - 1] = RF1ADOUT0B;
		}

		extern void radio_write_single_pa_table( uint8_t const & value );

		extern void radio_write_burst_pa_table( uint8_t const * const buffer, size_t const & count );

		extern void reset_core( );

		template<size_t BuffSize>
		class RadioCore {
		public:
			//			// RF States
			// 			enum class RFStates: uint8_t {
			// 				RFST_SFSTXON = 0x00,
			// 				RFST_SCAL = 0x01,
			// 				RFST_SRX = 0x02,
			// 				RFST_STX = 0x03,
			// 				RFST_SIDLE = 0x04
			// 			};
			typedef Buffer<uint8_t, BuffSize> buffer_t;
			
		private:

			struct RFFlags {
				volatile bool is_receiving;
				volatile bool has_received_packet;
				volatile bool receive_loop_on;

				RFFlags( ) : is_receiving( false ), has_received_packet( false ), receive_loop_on( false ) { }

				void reset( ) {
					is_receiving = false;
					has_received_packet = false;
					receive_loop_on = false;
				}
			} rf_flags;

			buffer_t rx_buffer;

		public:

			RadioCore( ) : rf_flags( ), rx_buffer( ) { }

			void receive_on( ) {
				RF1AIES |= BIT9;	// Falling edge of RFIFG9
				RF1AIFG &= ~BIT9;	// Clear a pending interrupt
				RF1AIE |= BIT9;	// Enable the interrupt

								// Radio is in IDLE following a TX, so strobe SRX to enter Receive Mode
				strobe( RF_SRX );	// Strobe SRX
				rf_flags.is_receiving = true;
			}

			bool is_receiving( ) const {
				return rf_flags.is_receiving;
			}

			typedef void( *config_fn_t )();
			void init_radio( config_fn_t configure_radio ) {
				rf_flags.reset( );
				rx_buffer.clear( );

				// Set the High-Power Mode Request Enable bit so LPM3 can be entered
				// with active radio enabled
				PMMCTL0_H = 0xA5;
				PMMCTL0_L |= PMMHPMRE_L;
				PMMCTL0_H = 0x00;

				configure_radio( );
			}

			void receive_off( ) {
				RF1AIE &= ~BIT9;                          // Disable RX interrupts
				RF1AIFG &= ~BIT9;                         // Clear pending IFG

														  // It is possible that ReceiveOff is called while radio is receiving a packet.
														  // Therefore, it is necessary to flush the RX FIFO after issuing IDLE strobe
														  // such that the RXFIFO is empty prior to receiving a packet.
				strobe( RF_SIDLE );
				strobe( RF_SFRX );
				rf_flags.is_receiving = false;
			}

			bool has_data( ) const {
				return !rx_buffer.empty( );
			}

			void check_for_data( ) {
				rf_flags.has_received_packet = true;
			}

			bool data_pending( ) const {
				return rf_flags.has_received_packet;
			}

			void reset_rx_buffer( ) {
				return rx_buffer.size( 0 );
			}

			size_t receive_data( ) {
				if( !rf_flags.has_received_packet ) {
					return 0;
				}
				size_t rx_len = radio_read_single_reg( RXBYTES );
				// For now clear buffer before proceeding and only read up to buffer len bytes
				rx_buffer.clear( );
				if( (rf_flags.has_received_packet = rx_len > rx_buffer.capacity( )) ) {
					rx_len = rx_buffer.capacity( );
				}
				rx_buffer.size( rx_len );
				radio_read_burst_reg( RF_RXFIFORD, rx_buffer, rx_len );

				return rx_len;
			}

			size_t size( ) const {
				return rx_buffer.size( );
			}

			size_t capacity( ) const {
				return rx_buffer.capacity( );
			}

			uint8_t const & rx_data( size_t const & position ) const {
				return rx_buffer[position];
			}

			uint8_t const * rx_data( ) const {
				return rx_buffer.data( );
			}

			buffer_t const & rx_array( ) const {
				return rx_buffer;
			}

			buffer_t & rx_array( ) {
				return rx_buffer;
			}
		};
	}
}
