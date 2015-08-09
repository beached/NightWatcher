#pragma once

#include <stdint.h>
#include <stddef.h>
#include "algorithm.h"
#include "array.h"

namespace daw {
	template<typename T, size_t max_size>
	class Buffer {
	public:
		typedef Array<T, max_size> values_type;
	private:
		size_t m_size;
		values_type m_buffer;

	public:
		typedef typename values_type::value_type value_type;
		typedef typename values_type::iterator iterator;
		typedef typename values_type::const_iterator const_iterator;
		typedef typename values_type::reference reference;
		typedef typename values_type::const_reference const_refernce;
		typedef typename values_type::size_type size_type;

		Buffer( ): m_size( 0 ), m_buffer( ) { }

		Buffer( T const & value ): m_size( 0 ), m_buffer( ) {
			fill_values( value );
		}

		iterator begin( ) {
			return m_buffer.begin( );
		}

		iterator const begin( ) const {
			return m_buffer.begin( );
		}

		const_iterator cbegin( ) const {
			return m_buffer.cbegin( );
		}

		iterator end( ) {
			return begin( ) + m_size;
		}

		iterator const end( ) const {
			return begin( ) + m_size;
		}

		const_iterator cend( ) {
			return begin( ) + m_size;
		}

		void fill_values( T const & value ) {
			daw::fill( m_buffer.begin( ), m_buffer.end( ), value );
		}

		void fill_values( T const & value ) volatile {
			daw::fill( m_buffer.begin( ), m_buffer.end( ), value );
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
			return 0 == m_size;
		}

		bool empty( ) const volatile {
			return 0 == m_size;
		}

		bool full( ) const {
			return max_size == m_size;
		}

		static size_t capacity( ) {
			return max_size;
		}

		size_t size( ) const {
			return m_size;
		}

		void size( size_t const & n ) {
			if( n <= max_size ) {
				m_size = n;
			}
		}

		size_t size( ) const volatile {
			return m_size;
		}

		void size( size_t const & n ) volatile {
			if( n <= max_size ) {
				m_size = n;
			}
		}

		T const & operator[]( size_t const & n ) const {
			return m_buffer[n];
		}

		T & operator[]( size_t const & n ) {
			return m_buffer[n];
		}

		T const & operator[]( size_t const & n ) const volatile {
			return m_buffer[n];
		}

		T & operator[]( size_t const & n ) volatile {
			return m_buffer[n];
		}

		bool push_back( T const & value ) {
			if( m_size >= max_size ) {
				return false;
			}
			m_buffer[m_size++] = value;
			return true;
		}

		// data( )
		T const * data( ) const {
			return m_buffer.data( );
		}

		T * data( ) {
			return m_buffer.data( );
		}

		T const * data( ) const volatile {
			return m_buffer.data( );
		}

		T * data( ) volatile {
			return m_buffer.data( );
		}
	};
}	// namespace daw
