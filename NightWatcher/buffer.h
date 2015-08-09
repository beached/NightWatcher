#pragma once

#include <stdint.h>
#include <stddef.h>
#include "algorithm.h"
#include "array.h"

namespace daw {
	template<typename T, size_t BUFFER_SIZE>
	class Buffer {
	public:
		typedef Array<T, BUFFER_SIZE> values_type;
		typedef typename values_type::value_type value_type;
		typedef typename values_type::iterator iterator;
		typedef typename values_type::const_iterator const_iterator;
		typedef typename values_type::reference reference;
		typedef typename values_type::const_reference const_refernce;
		typedef typename values_type::size_type size_type;
	private:
		size_t m_current_size;
		values_type m_array;
	public:
		Buffer( ): m_current_size( 0 ), m_array( ) { }

		Buffer( T const & value ): m_current_size( 0 ), m_array( ) {
			fill_values( value );
		}

		iterator begin( ) {
			return m_array.begin( );
		}

		iterator const begin( ) const {
			return m_array.begin( );
		}

		const_iterator cbegin( ) const {
			return m_array.cbegin( );
		}

		iterator end( ) {
			return begin( ) + m_current_size;
		}

		iterator const end( ) const {
			return begin( ) + m_current_size;
		}

		const_iterator cend( ) {
			return begin( ) + m_current_size;
		}

		void fill_values( T const & value ) {
			daw::fill( m_array.begin( ), m_array.end( ), value );
		}

		void fill_values( T const & value ) volatile {
			daw::fill( m_array.begin( ), m_array.end( ), value );
		}

		void zero_values( ) {
			fill_values( 0 );
		}

		void zero_values( ) volatile {
			fill_values( 0 );
		}

		void clear( ) {
			zero_values( );
			size( 0 );
		}

		void clear( ) volatile {
			zero_values( );
			size( 0 );
		}

		bool empty( ) const {
			return 0 == m_current_size;
		}

		bool empty( ) const volatile {
			return 0 == m_current_size;
		}

		bool full( ) const {
			return BUFFER_SIZE == m_current_size;
		}

		static size_t capacity( ) {
			return BUFFER_SIZE;
		}

		size_t size( ) const {
			return m_current_size;
		}

		void size( size_t const & n ) {
			if( n <= BUFFER_SIZE ) {
				m_current_size = n;
			}
		}

		size_t size( ) const volatile {
			return m_current_size;
		}

		void size( size_t const & n ) volatile {
			if( n <= BUFFER_SIZE ) {
				m_current_size = n;
			}
		}

		T const & operator[]( size_t const & n ) const {
			return m_array[n];
		}

		T & operator[]( size_t const & n ) {
			return m_array[n];
		}

		T const & operator[]( size_t const & n ) const volatile {
			return m_array[n];
		}

		T & operator[]( size_t const & n ) volatile {
			return m_array[n];
		}

		bool push_back( T const & value ) {
			if( m_current_size >= BUFFER_SIZE ) {
				return false;
			}
			m_array[m_current_size++] = value;
			return true;
		}

		// data( )
		T const * data( ) const {
			return m_array.data( );
		}

		T * data( ) {
			return m_array.data( );
		}

		T const * data( ) const volatile {
			return m_array.data( );
		}

		T * data( ) volatile {
			return m_array.data( );
		}
	};
}	// namespace daw
