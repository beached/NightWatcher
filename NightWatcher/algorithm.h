#pragma once

namespace daw {
	template< class ForwardIt, class T >
	void fill( ForwardIt first, ForwardIt last, const T& value ) {
		for( ; first != last; ++first ) {
			*first = value;
		}
	}
}
