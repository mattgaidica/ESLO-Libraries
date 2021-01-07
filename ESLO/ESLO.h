#ifndef _ESLO_H_
#define _ESLO_H_

#include <unistd.h>
#include <SPI_NAND.h>

#define ESLO_VERSION 0xAA
#define esloToNand(x,y) (memcpy(y, &x, sizeof(x)))

typedef enum { // 2bits, 0-3 (4 options)
	Mode_Debug, Mode_Deployment
} ESLO_Mode;

typedef enum { // 4bits, 0-15 (16 options)
	Type_AbsoluteTime,
	Type_RelativeTime,
	Type_EEG1,
	Type_EEG2,
	Type_EEG3,
	Type_EEG4,
	Type_BatteryVoltage,
	Type_AxyXlx,
	Type_AxyXly,
	Type_AxyXlz,
	Type_AxyMgx,
	Type_AxyMgy,
	Type_AxyMgz,
	Type_Temperature,
	Type_Error,
	Type_Version
} ESLO_Type;

typedef struct {
	uint8_t mode;
	uint8_t type;
	uint32_t data;
	uint32_t version;
} eslo_dt;

/*
 * uint32_t packet = 0xFF123456;
 * eslo_dt eslo;
 *
 * eslo.mode = Mode_Deployment;
 * eslo.type = Type_Temperature;
 * ESLO_Packet(eslo, &packet);
 */
void ESLO_Packet(eslo_dt eslo, uint32_t *packet);
ReturnType ESLO_Write(uAddrType *esloAddr, uint8_t *esloBuffer, eslo_dt eslo);

#endif //end of file
