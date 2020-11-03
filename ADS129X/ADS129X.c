// https://github.com/grafexum/ADS1115/blob/master/ads1115.h
// https://github.com/jcu-eresearch/c-hdc1080/blob/master/hdc1080.h

#include <ADS129X.h>
#include <math.h>
#include <unistd.h>

SPI_Handle ADS_init(uint_least8_t _index, uint_least8_t _csPin)
{
    SPI_Params spiParams;
    ADS_csPin = _csPin;
    SPI_Params_init(&spiParams);
    spiParams.frameFormat = SPI_POL0_PHA1;
    spiParams.bitRate = 1000000;
    return SPI_open(_index, &spiParams); // CONFIG_SPI_EEG
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

//HELPERS
void ADS_sendCommand(uint8_t _cmd)
{
    SPI_Transaction transaction;
    transaction.count = 1;
    transaction.txBuf = (void*) _cmd;
    transaction.rxBuf = NULL;
    GPIO_write(ADS_csPin, GPIO_CFG_OUT_LOW);
    transferOK = SPI_transfer(spiADS, &transaction);
    GPIO_write(ADS_csPin, GPIO_CFG_OUT_HIGH);
    // based on datasheet
    ADS_sleepTicks(4);
    if (_cmd == _ADS_WAKEUP)
    {
        ADS_sleepTicks(4);
    }
    else if (_cmd == _ADS_RESET)
    {
        ADS_sleepTicks(18);
    }
}

void ADS_sleepTicks(uint8_t n)
{
    usleep(ceil((444.0 * n) / 1000)); // tCLK = 444ns
}
