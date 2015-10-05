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
				struct __attribute__( (packed) ) glucometre {
					union {
						uint8_t raw[7];
						struct {
							uint8_t packet_type;	// always 0xA5
							uint32_t device_id : 24;
							uint16_t glucose_value;
							uint8_t crc;
						};
					};
				};

				struct __attribute__( (packed) ) sensor {
					union {
						uint8_t raw[33];
						struct {
							uint8_t packet_type;	// either 0xAA or 0xAB
							uint32_t device_id : 24;
							uint8_t version;	// always 13(0x0D)
							uint8_t unknown01;	// always 0x1D
							uint8_t isig_adjust;	// maybe 0x21
							uint8_t sequence_number;
							uint8_t recent_isig_values[4];
							uint8_t unknown02;	// usually 0x00. however, 0x02 when Change Sensor
							uint16_t unknown03;	// usually 0x6767. however, 0x0000 when Sensor Error
							uint8_t battery_level;	// usually 0x9D
							uint8_t past_isig_values[14];
							uint16_t crc;
						};
					};
				};
			}
		} // namespace medtronic
	} // namespace radio
}	// namespace daw
