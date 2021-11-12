#ifndef ____SK6812RGBW__
#define ____SK6812RGBW__

#include <SK6812RGBW.h>
// https://cpldcpu.wordpress.com/2014/01/14/light_ws2812-library-v2-0-part-i-understanding-the-ws2812/

/*
SPI_Handle LED_SPI_Init(uint8_t _index) {
	SPI_Handle SPI_handle;
	SPI_Params spiParams;
	SPI_Params_init(&spiParams);
	spiParams.bitRate = 8000000;
	spiParams.frameFormat = SPI_POL0_PHA0;
	SPI_handle = SPI_open(_index, &spiParams);
	return SPI_handle;
}

void sendBytes(uint8_t *Bufp, uint32_t len) {
	SPI_Transaction transaction;

	transaction.count = len;
	transaction.rxBuf = NULL;
	transaction.txBuf = (void*) Bufp;
	SPI_transfer(LED_SPI, &transaction);
}
*/

uint32_t readBit(uint32_t byte, uint32_t bit) {
	bit = 1 << bit;
	return (bit & byte);
}

void buildLedBitPattern(uint8_t R, uint8_t G, uint8_t B, uint8_t W,
		uint8_t *buf, uint8_t nLeds, uint32_t bitPattern) {
	int8_t i, iLED;
	uint32_t byteMask;
	uint32_t byteCount = 0;
	uint8_t T_LOW = 0xE0;
	uint8_t T_HIGH = 0xF8;

	// R
	for (iLED = 0; iLED < nLeds; iLED++) {
		if (readBit(bitPattern, iLED)) {
			byteMask = 0xFFFFFFFF;
		} else {
			byteMask = 0;
		}
		for (i = 7; i >= 0; i--) {
			if (readBit(G, i) & byteMask) {
				memcpy(buf + byteCount, &T_HIGH, 1);
			} else {
				memcpy(buf + byteCount, &T_LOW, 1);
			}
			byteCount++;
		}
		// G
		for (i = 7; i >= 0; i--) {
			if (readBit(R, i) & byteMask) {
				memcpy(buf + byteCount, &T_HIGH, 1);
			} else {
				memcpy(buf + byteCount, &T_LOW, 1);
			}
			byteCount++;
		}
		// B
		for (i = 7; i >= 0; i--) {
			if (readBit(B, i) & byteMask) {
				memcpy(buf + byteCount, &T_HIGH, 1);
			} else {
				memcpy(buf + byteCount, &T_LOW, 1);
			}
			byteCount++;
		}
		// W
		for (i = 7; i >= 0; i--) {
			if (readBit(W, i) & byteMask) {
				memcpy(buf + byteCount, &T_HIGH, 1);
			} else {
				memcpy(buf + byteCount, &T_LOW, 1);
			}
			byteCount++;
		}
	}
}
#endif
