#pragma once

#include "assert.h"
#include "bit.h"
#include <stddef.h>
#include <stdlib.h>

namespace daw {
	template<typename queue_type, typename value_type>
	class bit_queue_gen {
		queue_type m_queue;
		size_t m_size;

	public:
		bit_queue_gen( ): m_queue( 0 ), m_size( 0 ) { }

		size_t size( ) const {
			return m_size;
		}

		bool can_pop( size_t const bits ) const {
			return m_size >= bits;
		}

		bool empty( ) const {
			return 0 == m_size;
		}

		constexpr static size_t capacity( ) {
			return sizeof( m_queue ) * 8;
		}

		void push_back( value_type value, size_t const bits = sizeof( value_type ) * 8 ) {
			assert( capacity( ) - m_size >= bits, "Not enough bits to hold value pushed" );
			m_queue <<= bits;
			value &= get_mask<value_type>( bits );
			m_queue |= value;
			m_size += bits;
		}

		value_type pop_front( size_t const bits ) {
			queue_type const mask_pos = m_size - (bits - 1);
			auto result = static_cast<value_type>(m_queue >> (mask_pos - 1)); // right shift so that all but the left most 6bits are gone
			queue_type const mask = ~(get_mask<queue_type>( bits - 1 ) << (m_size - bits));
			m_queue &= mask;
			m_size -= bits;
			return result;
		}

		void clear( ) {
			m_queue = 0;
			m_size = 0;
		}

		value_type pop_all( ) {
			auto result = static_cast<value_type>(m_queue);
			clear( );
			return result;
		}
	};

	typedef bit_queue_gen<uint16_t, uint8_t> bit_queue;
	class nibble_queue {
		typedef uint16_t queue_type;
		typedef uint8_t value_type;
		bit_queue_gen<queue_type, value_type> m_queue;

	public:
		nibble_queue( ): m_queue( ) { }

		constexpr static size_t capacity( ) {
			return sizeof( queue_type ) / sizeof( value_type );
		}

		size_t size( ) const {
			return m_queue.size( ) / 4;
		}

		bool empty( ) const {
			return 0 == size( );
		}

		void push_back( value_type const &value ) {
			m_queue.push_back( value, 4 );
		}

		bool can_pop( size_t num_nibbles = sizeof( value_type ) * 2 ) const {
			return m_queue.can_pop( num_nibbles * 4 );
		}

		value_type pop_front( size_t num_nibbles = sizeof( value_type ) * 2 ) {
			return m_queue.pop_front( num_nibbles * 4 );
		}

		void clear( ) {
			m_queue.clear( );
		}

		value_type pop_all( ) {
			return m_queue.pop_all( );
		}
	};
}