#pragma once

#include <array>
#include <cc430f6137.h>

#ifndef TIMER0_CALLBACK_COUNT
#define TIMER0_CALLBACK_COUNT 0
#endif

#ifndef TIMER1_CALLBACK_COUNT
#define TIMER1_CALLBACK_COUNT 0
#endif

namespace timers {
#if TIMER0_CALLBACK_COUNT > 0
	class Timer0 {
	public:
		static std::array<void( *)(), TIMER0_CALLBACK_COUNT> s_timer_callbacks;

		Timer0( ) = delete;
		~Timer0( ) = delete;

		template<typename T>
		static void set_callback( T const & n, void( *callback )() ) {
			s_timer_callbacks[n] = callback;
		}

		template<typename T>
		static void clear_callback( T const & n ) {
			s_timer_callbacks[n] = nullptr;
		}

		template<typename T>
		static void enable( T const & countdown_from, bool const enable_interrupts = true ) {
			TA0CCTL0 = CCIE;	// CCR0 interrupt enabled
			TA0CCR0 = countdown_from;
			TA0CTL = TASSEL_2 + MC_1 + TACLR;	// SMCLK, up mode, clear TAR
			if( enable_interrupts ) {
				__bis_SR_register( LPM0_bits + GIE );	// Enter LPM0, enable interrupts
			}
		}
	};
#endif

#if TIMER1_CALLBACK_COUNT > 0
	class Timer1 {
	public:
		static std::array<void( *)(), TIMER1_CALLBACK_COUNT> s_timer_callbacks;

		Timer1( ) = delete;
		~Timer1( ) = delete;

		template<typename T>
		static void set_callback( T const & n, void( *callback )() ) {
			s_timer_callbacks[n] = callback;
		}

		template<typename T>
		static void clear_callback( T const & n ) {
			s_timer_callbacks[n] = nullptr;
		}

		template<typename T>
		static void enable( T const & countdown_from, bool const enable_interrupts = true ) {
			TA1CCTL0 = CCIE;	// CCR0 interrupt enabled
			TA1CCR0 = countdown_from;
			TA1CTL = TASSEL_2 + MC_1 + TACLR;	// SMCLK, up mode, clear TAR
			if( enable_interrupts ) {
				__bis_SR_register( LPM0_bits + GIE );	// Enter LPM0, enable interrupts
			}
		}
	};
#endif
}
