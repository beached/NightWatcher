#pragma once

#include "algorithm.h"
#include <stddef.h>

namespace daw {
	template<typename T, size_t ARRAY_SIZE>
	struct Array {
		typedef T value_type;
		typedef T* iterator;
		typedef T const* const_iterator;
		typedef T& reference;
		typedef T const& const_reference;
		typedef size_t size_type;

		typedef void( *init_fn )(T* const arry, size_t const& size_of);
	private:
		value_type m_values[ARRAY_SIZE];
	public:
		Array( ): m_values( ) { }

		explicit Array( T const& value ): m_values( ) {
			daw::fill( m_values, m_values + ARRAY_SIZE, value );
		}

		explicit Array( init_fn init_function ): m_values( ) {
			init_function( m_values, ARRAY_SIZE );
		}

		Array& operator=( Array const& other ) {
			if( this != &other ) {
				daw::copy( other.begin( ), other.end( ), begin( ) );
			}
			return *this;
		}

		static size_t size( ) {
			return ARRAY_SIZE;
		}

		iterator begin( ) {
			return m_values;
		}

		const_iterator begin( ) const {
			return m_values;
		}

		const_iterator cbegin( ) const {
			return m_values;
		}

		iterator end( ) {
			return m_values + ARRAY_SIZE;
		}

		const_iterator end( ) const {
			return m_values + ARRAY_SIZE;
		}

		const_iterator cend( ) const {
			return m_values + ARRAY_SIZE;
		}

		reference operator[]( size_t const& pos ) {
			return m_values[pos];
		}

		const_reference operator[]( size_t const& pos ) const {
			return m_values[pos];
		}

		value_type* data( ) {
			return m_values;
		}

		value_type const* data( ) const {
			return m_values;
		}

		static bool empty( ) {
			return 0 == ARRAY_SIZE;
		}
	};
}
