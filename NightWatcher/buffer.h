#pragma once

#include <array>
#include <cstdint>

template<typename T, size_t max_size>
class Buffer {
	size_t m_size;
	using values_t = std::array<T, max_size>;
	values_t m_buffer;

public:
	using iterator = typename values_t::iterator;
	using const_iterator = typename values_t::const_iterator;
	using reverse_iterator = typename values_t::reverse_iterator;
	using const_reverse_iterator = typename values_t::const_reverse_iterator;

	Buffer( ) : m_size( 0 ), m_buffer( ) { }

	Buffer( T const & value ) : m_size( 0 ), m_buffer( ) {
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

	reverse_iterator rbegin( ) {
		return m_buffer.rbegin( ) + (max_size - m_size);
	}

	reverse_iterator const rbegin( ) const {
		return m_buffer.rbegin( ) + (max_size - m_size);
	}

	const_reverse_iterator crbegin( ) {
		return m_buffer.crbegin( ) + (max_size - m_size);
	}

	reverse_iterator rend( ) {
		return m_buffer.rend( );
	}

	reverse_iterator const rend( ) const {
		return m_buffer.rend( );
	}

	const_reverse_iterator crend( ) {
		return m_buffer.rend( );
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
		std::fill( m_buffer.begin( ), m_buffer.end( ), value );
	}

	void fill_values( T const & value ) volatile {
		std::fill( m_buffer.begin( ), m_buffer.end( ), value );
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

	void size( size_t n ) {
		if( n <= max_size ) {
			m_size = std::move( n );
		}
	}

	size_t size( ) const volatile {
		return m_size;
	}

	void size( size_t n ) volatile {
		if( n <= max_size ) {
			m_size = std::move( n );
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

	bool push_back( T && value ) {
		if( m_size >= max_size ) {
			return false;
		}
		m_buffer[m_size++] = std::forward<T>( value );
		return true;
	}

	size_t push_back( T && value ) volatile {
		if( m_size >= max_size ) {
			return false;
		}
		m_buffer[m_size++] = std::forward<T>( value );
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
