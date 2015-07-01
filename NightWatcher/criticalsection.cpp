#include "CriticalSection.h"

CriticalSection::CriticalSection( ) : m_state( __get_interrupt_state( ) ) {
	__no_operation( );
	__disable_interrupt( );
}

CriticalSection::~CriticalSection( ) {
	__set_interrupt_state( m_state );
	__no_operation( );
	__enable_interrupt( );
	__no_operation( );
}