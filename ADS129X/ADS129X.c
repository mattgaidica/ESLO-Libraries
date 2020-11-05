// https://github.com/grafexum/ADS1115/blob/master/ads1115.h
// https://github.com/jcu-eresearch/c-hdc1080/blob/master/hdc1080.h
// !! handle transferOk?

#include <ADS129X.h>
#include <math.h>
#include <unistd.h>

void ADS_init(uint8_t _index, uint8_t _csPin)
{
    SPI_Params spiParams;
    ADS_csPin = _csPin;
    SPI_Params_init(&spiParams);
    spiParams.frameFormat = SPI_POL0_PHA1;
    spiParams.bitRate = 1000000;
    spiParams.dataSize = 8;
    spiADS = SPI_open(_index, &spiParams); // CONFIG_SPI_EEG

    ADS_wakeup();
    ADS_reset();
    usleep(9); // divide ticks by 2
    ADS_sdatac();
    ADS_wreg(_ADSreg_CONFIG3, 0xC0); // enable internal buffer
    ADS_wreg(_ADSreg_CONFIG1, 0x06); // LP mode, 250 SPS
    ADS_wreg(_ADSreg_CONFIG2, 0x10); // generate test internally
    ADS_wreg(_ADSreg_CH1SET, 0x80); // test signals
    ADS_wreg(_ADSreg_CH2SET, 0x80);
    ADS_wreg(_ADSreg_CH3SET, 0x05);
    ADS_wreg(_ADSreg_CH4SET, 0x80);
    // right now START pin is high, could be left floating and use commands?
    ADS_rdatac();
}

void ADS_close()
{
    ADS_standby();
    SPI_close(spiADS);
    // make CS input, remove pull-up from DRDY?
}

uint8_t ADS_getDeviceID()
{
    return ADS_rreg(_ADSreg_ID);
}

uint8_t ADS_rreg(uint8_t _address)
{
    // 0x00 = read 1 register
    uint8_t txBuffer[2] = { _ADS_RREG | _address, 0x00 };
    SPI_Transaction transaction;
    bool transferOK;
    ADS_sdatac();
    GPIO_write(ADS_csPin, GPIO_CFG_OUT_LOW);
    transaction.count = sizeof(txBuffer);
    transaction.txBuf = (void*) txBuffer;
    transaction.rxBuf = NULL;
    transferOK = SPI_transfer(spiADS, &transaction);

    uint8_t rxBuffer;
    uint8_t emptyBuffer[1] = { 0x00 };
    transaction.count = 1;
    transaction.txBuf = (void*) emptyBuffer;
    transaction.rxBuf = &rxBuffer;
    transferOK = SPI_transfer(spiADS, &transaction);
    GPIO_write(ADS_csPin, GPIO_CFG_OUT_HIGH);
    ADS_rdatac();
    return rxBuffer; // 0x90 for ADS1294
}

void ADS_wreg(uint8_t _address, uint8_t _value)
{
    // 0x00 = write 1 register
    uint8_t txBuffer[3] = { _ADS_WREG | _address, 0x00, _value };
    SPI_Transaction transaction;
    bool transferOK;
    ADS_sdatac();
    GPIO_write(ADS_csPin, GPIO_CFG_OUT_LOW);
    transaction.count = sizeof(txBuffer);
    transaction.txBuf = (void*) txBuffer;
    transaction.rxBuf = NULL;
    transferOK = SPI_transfer(spiADS, &transaction);
    ADS_rdatac();
}

void ADS_updateData(int32_t *status, int32_t *ch1, int32_t *ch2, int32_t *ch3,
                    int32_t *ch4)
{
    // call this from DRDY interrupt?
    uint8_t txBuffer[3] = { 0x00, 0x00, 0x00 };
    SPI_Transaction transaction;
    bool transferOk;
    int i;
    GPIO_write(ADS_csPin, GPIO_CFG_OUT_LOW);
    transaction.count = sizeof(txBuffer);
    transaction.txBuf = (void*)txBuffer;
    for (i = 1; i <= 5; i++)
    {
        switch (i)
        {
        case 1:
            transaction.rxBuf = (void*) status;
            break;
        case 2:
            transaction.rxBuf = (void*) ch1;
            break;
        case 3:
            transaction.rxBuf = (void*) ch2;
            break;
        case 4:
            transaction.rxBuf = (void*) ch3;
            break;
        case 5:
            transaction.rxBuf = (void*) ch4;
            break;
        }
        transferOk = SPI_transfer(spiADS, &transaction);
    }

    GPIO_write(ADS_csPin, GPIO_CFG_OUT_HIGH);
}

//SYSTEM COMMANDS
void ADS_standby()
{
    ADS_sendCommand(_ADS_WAKEUP);
}

void ADS_reset()
{
    ADS_sendCommand(_ADS_RESET);
}

void ADS_start()
{
    ADS_sendCommand(_ADS_START);
}

void ADS_stop()
{
    ADS_sendCommand(_ADS_STOP);
}

void ADS_wakeup()
{
    ADS_sendCommand(_ADS_WAKEUP);
}

void ADS_rdatac()
{
    ADS_sendCommand(_ADS_RDATAC);
}

void ADS_sdatac()
{
    ADS_sendCommand(_ADS_SDATAC);
}

void ADS_rdata()
{
    ADS_sendCommand(_ADS_RDATA);
}

//HELPERS
void ADS_sendCommand(uint8_t _cmd)
{
    SPI_Transaction transaction;
    bool transferOK;
    transaction.count = 1;
    transaction.txBuf = &_cmd;
    transaction.rxBuf = NULL;
    GPIO_write(ADS_csPin, GPIO_CFG_OUT_LOW);
    transferOK = SPI_transfer(spiADS, &transaction);
    GPIO_write(ADS_csPin, GPIO_CFG_OUT_HIGH);
    // !!check these requirements
    if (_cmd == _ADS_WAKEUP)
    {
        usleep(2);
    }
    else if (_cmd == _ADS_RESET)
    {
        usleep(9);
    }
}
