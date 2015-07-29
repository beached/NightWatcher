#pragma once

#include <cerrno>
#include <cstdint>
#include <cstring>

error_t memset_s( void* s, size_t smax, int c, size_t n );