#pragma once

#include <stdint.h>
#include <stddef.h>
#include "algorithm.h"

namespace daw {
	template<typename T, size_t array_size>
	struct Array {
		typedef T value_type;
		typedef T* iterator;
		typedef T const * const_iterator;
		typedef T& reference;
		typedef T const & const_reference;
		typedef size_t size_type;

		typedef void( *init_fn )(T* const arry, size_t const & size_of);
		value_type elems[array_size];

		Array( ): elems( ) { }
		Array( T const & value ): elems( ) {
			daw::fill( elems, elems + array_size, value );
		}

		Array( init_fn init_function ): elems( ) {
			init_function( elems, array_size );
		}

		static size_t size( ) {
			return array_size;
		}

		iterator begin( ) {
			return elems;
		}

		iterator const begin( ) const {
			return elems;
		}

		const_iterator cbegin( ) const {
			return elems;
		}

		iterator end( ) {
			return elems + array_size;
		}

		iterator const end( ) const {
			return elems + array_size;
		}

		const_iterator cend( ) const {
			return elems + array_size;
		}

		reference operator[]( size_t const & pos ) {
			return elems[pos];
		}

		const_reference operator[]( size_t const & pos ) const {
			return elems[pos];
		}

		value_type * data( ) {
			return elems;
		}

		value_type const * data( ) const {
			return elems;
		}

		static bool empty( ) {
			return 0 == array_size;
		}
	};
}