//
//  ADS1299.h
//  
//  Created by Conor Russomanno on 6/17/13.
//

#ifndef ____ADS129X__
#define ____ADS129X__

#include "Definitions.h"
#include "ti_drivers_config.h"
#include <ti/drivers/SPI.h>
#include <ti/drivers/GPIO.h>

bool transferOK;
uint_least8_t ADS_csPin;
SPI_Handle spiADS;

//Helpers
void ADS_sleepTicks(uint8_t n);
void ADS_sendCommand(uint8_t _cmd);

//System Commands
SPI_Handle ADS_init(uint_least8_t _index, uint_least8_t _csPin);
void ADS_wakeup();
void ADS_standby();
void ADS_reset();
void ADS_start();
void ADS_stop();

//Data Read Commands
void ADS_rdatac();
void ADS_sdatac();
void ADS_rdata();

//Register Read/Write Commands
void ADS_getDeviceID();
void ADS_rreg(uint8_t _address, uint8_t _numRegistersMinusOne);
void ADS_wreg(uint8_t _address, uint8_t _value, uint8_t _numRegistersMinusOne);

void ADS_updateData();

#endif
