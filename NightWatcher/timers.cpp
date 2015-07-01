#include "timers.h"

#if TIMER1_CALLBACK_COUNT > 0

#include <csignal>
#include <array>
#include <cc430f6137.h>

namespace timers {
	void Timer1::set_callback( size_t n, void( *callback )() ) {
		s_timer_callbacks[n] = callback;
	}

	void Timer1::clear_callback( size_t n ) {
		s_timer_callbacks[n] = nullptr;
	}

	void Timer1::enable( size_t countdown_from ) {
		TA1CCTL0 = CCIE;                          // CCR0 interrupt enabled
		TA1CCR0 = countdown_from;	// std::numeric_limits<decltype(TA1CCR0)>::max( );
		TA1CTL = TASSEL_2 + MC_1 + TACLR;         // SMCLK, upmode, clear TAR
	}

	std::array<void( *)(), TIMER1_CALLBACK_COUNT> Timer1::s_timer_callbacks = { nullptr };
}

__attribute__( (interrupt( TIMER1_A0_VECTOR )) ) void timer_isr( ) {
	using namespace timers;
	for( size_t n = 0; n < Timer1::s_timer_callbacks.size( ); ++n ) {
		if( !Timer1::s_timer_callbacks[n] ) {
			break;
		}
		Timer1::s_timer_callbacks[n]( );
	}
}
#endif