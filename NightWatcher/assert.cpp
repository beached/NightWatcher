#include "assert.h"
#include <intrinsics.h>

namespace daw {
	void assert( bool const condition, char const * const msg ) {
	#ifdef DEBUG
		while( !condition ) {
			// Spin
			__no_operation( );
		}
	#endif
	}
}