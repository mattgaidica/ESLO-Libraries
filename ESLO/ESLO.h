#ifndef _ESLO_H_
#define _ESLO_H_

#include <unistd.h>
#include <SPI_NAND.h>

#define ESLO_REVISION 0xAA // !! how to use alongside version?
//#define esloToNand(x,y) (memcpy(y, &x, sizeof(x))) // deprecate
#define VERSION_LENGTH 3

typedef enum {
	ESLO_LOW,
	ESLO_HIGH
} ESLO_States;

typedef enum {
	ESLO_MODULE_OFF,
	ESLO_MODULE_ON
} ESLO_ModuleStatus;

// mode could also be rec. freq. for Axy using or-flags
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
	Type_Error, // needed?
	Type_Version
} ESLO_Type;

typedef enum {
	Set_SleepWake,
	Set_EEGDuty,
	Set_EEGDuration,
	Set_EEG1,
	Set_EEG2,
	Set_EEG3,
	Set_EEG4,
	Set_AxyMode,
	Set_TxPower,
	Set_Time1,
	Set_Time2,
	Set_Time3,
	Set_Time4
} ESLO_Settings;

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
void ESLO_SetVersion(uint32_t *esloVersion, uint_least8_t index);

#endif //end of file
