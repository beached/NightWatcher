#pragma once

#include <stdlib.h>

namespace daw {
	namespace radio {
		namespace medtronic {
			namespace packets {
				namespace types {
					enum MedtronicPacketType: uint8_t {
						Pump = 0xA2,
						Glucometre = 0xA5,
						EnliteSensorWarmup = 0xAA,
						EnliteSensor = 0xAB
					};
				}
				struct glucometre {
					union {
						uint8_t raw[7];
						struct {
							uint8_t packet_type;
							uint32_t device_id : 24;
							uint16_t glucose_value;
							uint8_t crc;
						};
					};
				} __attribute__( (packed) );
			}
		} // namespace medtronic
	} // namespace radio
}	// namespace daw
