#include "timers.h"

#include <csignal>
#include <array>
#include <cc430f6137.h>

#if TIMER0_CALLBACK_COUNT > 0
std::array<void( *)(), TIMER0_CALLBACK_COUNT> timers::Timer0::s_timer_callbacks = { nullptr };

__attribute__( (interrupt( TIMER0_A0_VECTOR )) ) void timer_isr( ) {
	using namespace timers;
	for( size_t n = 0; n < Timer0::s_timer_callbacks.size( ); ++n ) {
		if( !Timer0::s_timer_callbacks[n] ) {
			break;
		}
		Timer0::s_timer_callbacks[n]( );
	}
}
#endif

#if TIMER1_CALLBACK_COUNT > 0
std::array<void( *)(), TIMER1_CALLBACK_COUNT> timers::Timer1::s_timer_callbacks = { nullptr };

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