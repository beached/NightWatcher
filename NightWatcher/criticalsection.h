#pragma once

#include <cc430f6137.h>
#include <cstdint>

class CriticalSection {
public:
	CriticalSection( );
	~CriticalSection( );
	CriticalSection( CriticalSection const & ) = delete;
	CriticalSection( CriticalSection && ) = delete;
	CriticalSection& operator=( CriticalSection const & ) = delete;
	CriticalSection& operator=( CriticalSection && ) = delete;
private:
	uint16_t m_state;
};