#pragma once

#ifndef TIMER1_CALLBACK_COUNT
#define TIMER1_CALLBACK_COUNT 0
#endif

#if TIMER1_CALLBACK_COUNT > 0

#include <array>
#include <functional>
#include <cc430f6137.h>

namespace timers {
	class Timer1 {
	public:
		static void set_callback( size_t n, void( *callback )() );
		static void clear_callback( size_t n );
		static void enable( size_t countdown_from );

		static std::array<void( *)(), TIMER1_CALLBACK_COUNT> s_timer_callbacks;
	};
}

#endif