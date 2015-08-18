#include "radio_medtronic.h"
#include "radio_core.h"
#include <stdint.h>
#include "buffer.h"
#include "array.h"
#include "crc4b6b.h"

namespace daw {
	namespace radio {
		namespace medtronic {
			radio_data_buffer_t radio_data_buffer;
			size_t radio_data_buffer_size = 0;
		} // namespace medtronic
	} // namespace radio
}	// namespace daw