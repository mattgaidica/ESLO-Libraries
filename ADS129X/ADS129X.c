// https://github.com/grafexum/ADS1115/blob/master/ads1115.h
// https://github.com/jcu-eresearch/c-hdc1080/blob/master/hdc1080.h
// !! handle transferOk?

#include <ADS129X.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <math.h>
#include <unistd.h>

void ADS_init(uint8_t _index, uint8_t _csPin) {
	SPI_Params spiParams;
	ADS_csPin = _csPin;
	SPI_Params_init(&spiParams);
	spiParams.frameFormat = SPI_POL0_PHA1;
	spiParams.bitRate = 1000000;
	spiParams.dataSize = 8;
	spiParams.transferTimeout = 10; // !!what is a good value?
	spiADS = SPI_open(_index, &spiParams); // CONFIG_SPI_EEG

//    ADS_wakeup(); // do we ever want to use this?
	ADS_reset();
	ADS_sdatac();
	ADS_wreg(_ADSreg_CONFIG3, 0xCC); // enable internal buffer
	ADS_wreg(_ADSreg_CONFIG1, 0x06); // LP mode, 250 SPS
	ADS_wreg(_ADSreg_CONFIG2, 0x10); // generate test internally, faster mode
	ADS_wreg(0x0D, 0x00); //use all chs for RLD, also CONFIG3
	ADS_wreg(0x0E, 0x00); //use all chs for RLD, also CONFIG3
	// right now START pin is high, could be left floating and use commands?
	ADS_rdatac();
}

void ADS_enableChannels(bool Ch1, bool Ch2, bool Ch3, bool Ch4) {
	ADS_sdatac();
	if (Ch1) {
		ADS_wreg(_ADSreg_CH1SET, 0x60);
	} else {
		ADS_wreg(_ADSreg_CH1SET, 0x81); // channel power-down, MUXn[2:0]=b001
	}
	if (Ch2) {
		ADS_wreg(_ADSreg_CH2SET, 0x60);
	} else {
		ADS_wreg(_ADSreg_CH2SET, 0x81); // channel power-down, MUXn[2:0]=b001
	}
	if (Ch3) {
		ADS_wreg(_ADSreg_CH3SET, 0x60);
	} else {
		ADS_wreg(_ADSreg_CH3SET, 0x81); // channel power-down, MUXn[2:0]=b001
	}
	if (Ch4) {
		ADS_wreg(_ADSreg_CH4SET, 0x60);
	} else {
		ADS_wreg(_ADSreg_CH4SET, 0x81); // channel power-down, MUXn[2:0]=b001
	}
	ADS_rdatac();
}

void ADS_close() {
	ADS_standby();
	SPI_close(spiADS);
	// make CS input, remove pull-up from DRDY?
}

uint8_t ADS_getDeviceID() {
	return ADS_rreg(_ADSreg_ID);
}

uint8_t ADS_rreg(uint8_t _address) {
	// 0x00 = read 1 register
	uint8_t txBuffer[2] = { _ADS_RREG | _address, 0x00 };
	SPI_Transaction transaction;
//	bool transferOK;
	ADS_sdatac();
	GPIO_write(ADS_csPin, GPIO_CFG_OUT_LOW);
	transaction.count = sizeof(txBuffer);
	transaction.txBuf = (void*) txBuffer;
	transaction.rxBuf = NULL;
	SPI_transfer(spiADS, &transaction);

	uint8_t rxBuffer;
	uint8_t emptyBuffer[1] = { 0x00 };
	transaction.count = 1;
	transaction.txBuf = (void*) emptyBuffer;
	transaction.rxBuf = &rxBuffer;
	SPI_transfer(spiADS, &transaction);
	GPIO_write(ADS_csPin, GPIO_CFG_OUT_HIGH);
	ADS_rdatac();
	return rxBuffer; // 0x90 for ADS1294
}

void ADS_wreg(uint8_t _address, uint8_t _value) {
	// 0x00 = write 1 register
	uint8_t txBuffer[3] = { _ADS_WREG | _address, 0x00, _value };
	SPI_Transaction transaction;
//	bool transferOK;
	ADS_sdatac();
	GPIO_write(ADS_csPin, GPIO_CFG_OUT_LOW);
	transaction.count = sizeof(txBuffer);
	transaction.txBuf = (void*) txBuffer;
	transaction.rxBuf = NULL;
	SPI_transfer(spiADS, &transaction);
	ADS_rdatac();
}

void ADS_updateData(int32_t *status, int32_t *ch1, int32_t *ch2, int32_t *ch3,
		int32_t *ch4) {
	uint8_t txBuffer[3] = { 0x00, 0x00, 0x00 };
	uint8_t rxBuffer[3];
	SPI_Transaction transaction;
//	bool transferOk;
	int i;
	GPIO_write(ADS_csPin, GPIO_CFG_OUT_LOW);
	transaction.count = sizeof(txBuffer);
	transaction.txBuf = (void*) txBuffer;
	transaction.rxBuf = (void*) rxBuffer;
	for (i = 1; i <= 5; i++) {
		SPI_transfer(spiADS, &transaction);
		int32_t setValue = (rxBuffer[0] << 16 | rxBuffer[1] << 8 | rxBuffer[2]);
		switch (i) {
		case 1:
			*status = setValue; // not sure if STATUS is needed
			break;
		case 2:
			*ch1 = sign32(setValue);
			break;
		case 3:
			*ch2 = sign32(setValue);
			break;
		case 4:
			*ch3 = sign32(setValue);
			break;
		case 5:
			*ch4 = sign32(setValue);
			break;
		}
	}

	GPIO_write(ADS_csPin, GPIO_CFG_OUT_HIGH);
}

//SYSTEM COMMANDS
void ADS_standby() {
	ADS_sendCommand(_ADS_WAKEUP);
}

void ADS_reset() {
	ADS_sendCommand(_ADS_RESET);
}

void ADS_start() {
	ADS_sendCommand(_ADS_START);
}

void ADS_stop() {
	ADS_sendCommand(_ADS_STOP);
}

void ADS_wakeup() {
	ADS_sendCommand(_ADS_WAKEUP);
}

void ADS_rdatac() {
	ADS_sendCommand(_ADS_RDATAC);
}

void ADS_sdatac() {
	ADS_sendCommand(_ADS_SDATAC);
}

void ADS_rdata() {
	ADS_sendCommand(_ADS_RDATA);
}

//HELPERS
int32_t sign32(int32_t val) {
	if (val & 0x00800000) {
		return val |= 0xFF000000;
	} else {
		return val;
	}
}

void ADS_sendCommand(uint8_t _cmd) {
	SPI_Transaction transaction;
//	bool transferOK;
	transaction.count = 1;
	transaction.txBuf = &_cmd;
	transaction.rxBuf = NULL;
	GPIO_write(ADS_csPin, GPIO_CFG_OUT_LOW);
	SPI_transfer(spiADS, &transaction);
	GPIO_write(ADS_csPin, GPIO_CFG_OUT_HIGH);
	// delay next command 4tclks
	Task_sleep(2 / Clock_tickPeriod); // N*10 usec, 4tclk (tclk = 514nS)
	if (_cmd == _ADS_RESET) {
		Task_sleep(10 / Clock_tickPeriod); // N*10 usec, 18tclk (9.2uS)
	}
}
