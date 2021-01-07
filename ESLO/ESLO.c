#include <ESLO.h>
#include <SPI_NAND.h>
#include <string.h>

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

	// !! if page is not updating, this line must not be working?
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

		if (ADDRESS_2_PAGE(*esloAddr) == 0) { // fresh block
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
