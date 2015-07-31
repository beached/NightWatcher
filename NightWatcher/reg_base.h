#pragma once
#include <cstdint>

namespace daw {
	namespace policies {
		template<typename ValueType>
		struct wo_t {
			static void write( volatile ValueType* const reg, size_t mask, size_t offset, ValueType const & value ) {
				*reg = (value & mask) << offset;
			}
		};

		using wo_8bit_t = wo_t<uint8_t>;
		using wo_16bit_t = wo_t<uint16_t>;

		template<typename ValueType>
		struct ro_t {
			static ValueType read( volatile ValueType const * const reg, size_t const & mask, size_t const & offset ) {
				return (*reg >> offset) & mask;
			}
		};

		using ro_8bit_t = ro_t<uint8_t>;
		using ro_16bit_t = ro_t<uint16_t>;

		template<typename ValueType>
		struct rw_t: public ro_t<ValueType> {
			static void write( volatile ValueType * const reg, size_t const & mask, size_t const & offset ) {
				*reg = (*reg & ~(mask << offset)) | ((value & mask) << offset);
			}
		};

		using rw_8bit_t = rw_t<uint8_t>;
		using rw_16bit_t = rw_t<uint16_t>;
	}

	template<size_t address, size_t mask, size_t offset, typename mutability_policy_8bit>
	struct reg_8bit_t {
		static void write( uint8_t const & value ) {
			mutability_policy::write( reinterpret_cast<volatile uint8_t*>(address), mask, offset, value );
		}

		static auto read( ) -> decltype(mutability_policy::read( reinterpret_cast<volatile uint8_t*>(address), mask, offset )) {
			return mutability_policy::read( reinterpret_cast<volatile uint8_t*>(address), mask, offset );
		}
	};

	template<size_t address, size_t mask, size_t offset, typename mutability_policy_16bit>
	struct reg_16bit_t {
		static void write( uint16_t value ) {
			mutability_policy::write( reinterpret_cast<volatile uint16_t*>(address), mask, offset, value );
		}

		static auto read( ) -> decltype(mutability_policy::read( reinterpret_cast<volatile uint16_t*>(address), mask, offset )) {
			return mutability_policy::read( reinterpret_cast<volatile uint16_t*>(address), mask, offset );
		}
	};
}