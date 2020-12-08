#ifndef _ESLO_H_
#define _ESLO_H_

#include <unistd.h>

#define ESLO_VERSION 0xAA

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
	Type_Accelerometer,
	Type_Temperature
} ESLO_Type;

typedef struct {
	uint8_t mode;
	uint8_t type;
	uint32_t *data; /* pointer to data buffer */
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

#endif //end of file
