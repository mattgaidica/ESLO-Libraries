#ifndef _SERIALIZE_H_ 
#define _SERIALIZE_H_ 

#include <unistd.h>
#include <SPI_NAND.h>

#include <ti/drivers/SPI.h>
#include <ti/drivers/GPIO.h>
#include <ti_drivers_config.h>

#define NULL_PTR 0x0   // a null pointer
#define NAND_PASS 1
#define NAND_FAIL 0

SPI_Handle SPI_MEM_handle;
SPI_Params spiParams;

// Matt: rm these and included stdint.h
//typedef unsigned char uint8;
//typedef signed char sint8;
//typedef unsigned int uint16;
//typedef int sint16;
//typedef unsigned long uint32;
//typedef long sint32;

typedef enum {
	RetSpiError, RetSpiSuccess
} SPI_STATUS;

//typedef unsigned char Bool;

// Acceptable values for SPI master side configuration 
typedef enum _SpiConfigOptions {
	OpsNull,  			// do nothing 
	OpsWakeUp,			// enable transfer 
	OpsInitTransfer,
	OpsEndTransfer,

} SpiConfigOptions;

// char stream definition for 
typedef struct _structCharStream {
	uint8_t *pChar;                      // buffer address that holds the streams
	uint32_t length;                             // length of the stream in bytes
} CharStream;

bool NAND_Init();

SPI_STATUS Serialize_SPI(const CharStream *char_stream_send,
		CharStream *char_stream_recv, SpiConfigOptions optBefore,
		SpiConfigOptions optAfter);

void ConfigureSpi(SpiConfigOptions opt);

#endif //end of file 
