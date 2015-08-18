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
		} // namespace medtronic
	} // namespace radio
}	// namespace daw
