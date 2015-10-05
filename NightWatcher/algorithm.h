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

	template<class InputIt, class OutputIt, class UnaryOperation>
	OutputIt transform( InputIt first1, InputIt last1, OutputIt d_first, UnaryOperation unary_op ) {
		while( first1 != last1 ) {
			*d_first++ = unary_op( *first1++ );
		}
		return d_first;
	}

	template<class InputIt1, class InputIt2, class OutputIt, class BinaryOperation>
	OutputIt transform( InputIt1 first1, InputIt1 last1, InputIt2 first2, OutputIt d_first, BinaryOperation binary_op ) {
		while( first1 != last1 ) {
			*d_first++ = binary_op( *first1++, *first2++ );
		}
		return d_first;
	}

	template<class InputIt, class T>
	T accumulate( InputIt first, InputIt last, T init ) {
		for( ; first != last; ++first ) {
			init = init + *first;
		}
		return init;
	}

	template<class InputIt, class T, class BinaryOperation>
	T accumulate( InputIt first, InputIt last, T init,
		BinaryOperation op ) {
		for( ; first != last; ++first ) {
			init = op( init, *first );
		}
		return init;
	}
	
}
