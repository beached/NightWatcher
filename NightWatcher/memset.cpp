#include "memset.h"

error_t memset_s( void* s, size_t smax, int c, size_t n ) {
	error_t err = 0;
	if( n < 1 ) {
		return err;
	}
	if( nullptr == s ) {
		return EINVAL;
	}
	if( smax > SIZE_MAX ) {
		return E2BIG;
	}
	if( n > SIZE_MAX ) {
		err = E2BIG;
		n = smax;
	}
	if( n > smax ) {
		err = EOVERFLOW;
		n = smax;
	}
	memset( s, c, n );
	return err;
}