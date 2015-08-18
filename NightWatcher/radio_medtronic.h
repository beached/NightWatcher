#pragma once

#include <stdint.h>
#include "buffer.h"
#include "intrinsics.h"

namespace daw {
	namespace radio {
		namespace medtronic {
			typedef Buffer<uint8_t, 64> radio_data_buffer_t;
			extern radio_data_buffer_t radio_data_buffer;
			extern size_t radio_data_buffer_size;
			namespace MedtronicPacketTypes {
				enum MedtronicPacketType: uint8_t {
					Pump = 0xA2,
					Glucometre = 0xA5,
					EnliteSensorWarmup = 0xAA,
					EnliteSensor = 0xAB
				};
			}

			//MedtronicPacketTypes::MedtronicPacketType determine_packet_type( BufferType const & rdo_buffer );
		} // namespace medtronic
	} // namespace radio
}	// namespace daw
