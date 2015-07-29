#pragma once

#include <cstdint>
#include <cstddef>

uint8_t crc8( uint8_t *message, size_t nBytes );
void crc16_init( );
int16_t crc16( uint8_t *message, size_t length );
uint8_t get_4b6b_symbol( uint8_t const & in_symbol );
uint8_t decode_4b6b_symbol( uint8_t const & in_symbol, uint8_t & out_symbol );
void encode_4b6b( uint8_t *in_message, size_t in_length, uint8_t *out_message, size_t & out_length );
void decode_4b6b( uint8_t *in_message, size_t const & in_length, uint8_t *out_message, size_t & out_length );

// void sleep( int16_t time );
