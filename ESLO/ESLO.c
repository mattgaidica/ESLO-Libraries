#include <ESLO.h>
#include <SPI_NAND.h>
#include <string.h>
#include <ti/drivers/TRNG.h>
#include <ti/drivers/cryptoutils/cryptokey/CryptoKeyPlaintext.h>

void ESLO_SetVersion(uint32_t *esloVersion, uint_least8_t index) {
	TRNG_Handle rndHandle;
	int_fast16_t rndRes;
	CryptoKey entropyKey;
	uint8_t entropyBuffer[VERSION_LENGTH]; // 24-bits
	TRNG_init();
	rndHandle = TRNG_open(index, NULL);
	if (!rndHandle) {
		// Handle error
		while (1)
			;
	}
	CryptoKeyPlaintext_initBlankKey(&entropyKey, entropyBuffer, VERSION_LENGTH);
	rndRes = TRNG_generateEntropy(rndHandle, &entropyKey);
	if (rndRes != TRNG_STATUS_SUCCESS) {
		// Handle error
		while (1)
			;
	}
	TRNG_close(rndHandle);

	// set version
	memcpy(esloVersion, entropyBuffer, sizeof(entropyBuffer));
}

void ESLO_Packet(eslo_dt eslo, uint32_t *packet) {
	*packet = (eslo.data & 0x00FFFFFF)
			| // make data 24-bits
			((uint32_t) (eslo.type << 24) & 0x0F000000)
			| ((uint32_t) (eslo.mode << 28) & 0xF0000000);
}

ReturnType ESLO_Write(uAddrType *esloAddr, uint8_t *esloBuffer, eslo_dt eslo) {
	ReturnType ret;
	uint32_t packet;

	ESLO_Packet(eslo, &packet);

	if (*esloAddr >= FLASH_SIZE)
		ret = Flash_MemoryOverflow;

	memcpy(esloBuffer + ADDRESS_2_COL(*esloAddr), &packet, 4);
	if (ADDRESS_2_COL(*esloAddr) == PAGE_DATA_SIZE) { // end of page, write it
		if (ADDRESS_2_PAGE(*esloAddr) == 0) { // first page in block
			ret = FlashBlockErase(*esloAddr); // wipe 64 pages
			if (ret != Flash_Success) {
				return ret; // return early
			}
		}

		(*esloAddr) &= 0xFFFFF000; // always write to 0th column index
		ret = FlashPageProgram(*esloAddr, esloBuffer, PAGE_DATA_SIZE); // write page
		(*esloAddr) += 0x00001000; // increment page

		// fresh block, still requires an intentional version write at startup
		if (ADDRESS_2_PAGE(*esloAddr) == 0) {
			eslo.type = Type_Version;
			eslo.data = eslo.version;
			ESLO_Packet(eslo, &packet);
			memcpy(esloBuffer + ADDRESS_2_COL(*esloAddr), &packet, 4);
			*esloAddr += 4;
		}
	} else {
		*esloAddr += 4; // next mem location, add sizeof(packet)
		ret = Flash_Success;
	}

	return ret;
}
