#pragma once

#include <cc430f6137.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "buffer.h"
#include "algorithm.h"

#define _HAL_PMM_DISABLE_SVML_
#define _HAL_PMM_SVMLE SVMLE
#define _HAL_PMM_DISABLE_SVSL_
#define _HAL_PMM_SVSLE SVSLE
#define _HAL_PMM_DISABLE_FULL_PERFORMANCE_
#define _HAL_PMM_SVSFP SVSLFP

namespace daw {
	namespace radio {
		namespace core {
			uint8_t const PMM_STATUS_OK = 0;
			uint8_t const PMM_STATUS_ERROR = 1;

			extern void init_fll( uint16_t fsystem, uint16_t ratio ); 
			extern uint16_t set_vcore_up( uint16_t const & level );
			extern uint16_t set_vcore_down( uint16_t const & level );
			extern uint16_t set_vcore( uint16_t level );
			extern uint8_t strobe( uint8_t const cmd );
			extern void radio_write_single_reg( uint8_t const & addr, uint8_t const & value );
			extern uint8_t radio_read_single_reg( uint8_t const & addr );
			extern void radio_write_single_pa_table( uint8_t const & value );
			extern void radio_write_burst_pa_table( uint8_t const * const buffer, size_t const & count );
			extern void reset_core( );

			template<typename ArryType>
			static void radio_read_burst_reg( uint8_t const & addr, ArryType & buffer, uint8_t const & count ) {
				while( !(RF1AIFCTL1 & RFINSTRIFG) ) { /* spin */ }
				RF1AINSTR1B = (addr | RF_REGRD);
				daw::fill( buffer.begin( ), buffer.begin( ) + count, RF1ADOUT1B );
				buffer[count - 1] = RF1ADOUT0B;
			}
	
			
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
					RF1AIES |= BIT9; // Falling edge of RFIFG9
					RF1AIFG &= ~BIT9; // Clear a pending interrupt
					RF1AIE |= BIT9; // Enable the interrupt
	
									// Radio is in IDLE following a TX, so strobe SRX to enter Receive Mode
					strobe( RF_SRX ); // Strobe SRX
					rf_flags.is_receiving = true;
				}
	
				bool is_receiving( ) const {
					return rf_flags.is_receiving;
				}
	
				typedef void( *config_fn_t )();
				void init_radio( config_fn_t configure_radio ) {
					rf_flags.reset( );
					rx_buffer.clear( ); // Set the High-Power Mode Request Enable bit so LPM3 can be entered
					// with active radio enabled
					PMMCTL0_H = 0xA5;
					PMMCTL0_L |= PMMHPMRE_L;
					PMMCTL0_H = 0x00;
	
					configure_radio( );
				}
	
				void receive_off( ) {
					RF1AIE &= ~BIT9; // Disable RX interrupts
					RF1AIFG &= ~BIT9; // Clear pending IFG
	
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
					size_t rx_len = radio_read_single_reg( RXBYTES ); // For now clear buffer before proceeding and only read up to buffer len bytes
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
		}	// namespace core
	} // namespace radio
} // namespace daw
