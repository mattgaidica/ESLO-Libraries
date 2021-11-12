#include <unistd.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

void buildLedBitPattern(uint8_t R, uint8_t G, uint8_t B, uint8_t W,
		uint8_t *buf, uint8_t nLeds, uint32_t bitPattern);

uint32_t readBit(uint32_t byte, uint32_t bit);
