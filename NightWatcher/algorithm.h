#pragma once

#include <stddef.h>
#include <stdlib.h>

namespace daw {
	template<class ForwardIt, class T>
	void fill( ForwardIt first, ForwardIt last, const T& value ) {
		for( ; first != last; ++first ) {
			*first = value;
		}
	}

	template<class ForwardIt, class T>
	void fill( ForwardIt first, ForwardIt last, const volatile T& value ) {
		for( ; first != last; ++first ) {
			*first = value;
		}
	}

	template<class InputIterator, class OutputIterator>
	OutputIterator copy( InputIterator first, InputIterator last, OutputIterator result ) {
		while( first != last ) {
			*result = *first;
			++result;
			++first;
		}
		return result;
	}

	template<typename Container>
	constexpr auto begin( Container & c ) -> decltype(c.begin( )) {
		return c.begin( );
	}

	template<typename Container>
	constexpr auto begin( Container const & c ) -> decltype(c.begin( )) {
		return c.begin( );
	}

	template<typename T, size_t arry_size>
	constexpr T * begin( T( &array )[arry_size] ) {
		return array;
	}

	template<typename Container>
	constexpr auto end( Container & c ) -> decltype(c.end( )) {
		return c.end( );
	}

	template<typename Container>
	constexpr auto end( Container const & c ) -> decltype(c.end( )) {
		return c.end( );
	}

	template<typename T, size_t arry_size>
	constexpr T * end( T( &array )[arry_size] ) {
		return array + arry_size;
	}

	template<typename T, size_t arry_size>
	constexpr size_t sizeof_array( const T( &)[arry_size] ) {
		return arry_size;
	}

	template<class InputIterator, class Function>
	Function for_each( InputIterator first, InputIterator last, Function fn ) {
		while( first != last ) {
			fn( *first );
			++first;
		}
		return fn;
	}
}
