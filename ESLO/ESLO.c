#include <ESLO.h>

void ESLO_Packet(eslo_dt eslo, uint32_t *packet) {
	*packet = (*packet & 0x00FFFFFF)
			| // make data 24-bits
			((uint32_t)(eslo.type << 24) & 0x0F000000)
			| ((uint32_t)(eslo.mode << 28) & 0xF0000000);
}
