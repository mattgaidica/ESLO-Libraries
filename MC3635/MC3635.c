#include "MC3635.h"
#include <ti/drivers/SPI.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stddef.h>

uint8_t CfgRange, CfgResolution, CfgFifo, CfgINT;

// https://github.com/mcubemems/mCube_mc36xx_arduino_driver/blob/master/MC36XX.cpp

SPI_Handle MC3635_init(uint_least8_t CONFIG_SPI)
{
    SPI_Handle spiHandle;
    SPI_Params spiParams;

    SPI_Params_init(&spiParams);
    spiParams.frameFormat = SPI_POL1_PHA1;
    spiParams.bitRate = 100000;
    spiHandle = SPI_open(CONFIG_SPI, &spiParams);
    return spiHandle;
}

bool MC3635_start(SPI_Handle spiHandle)
{
    MC3635_reset(spiHandle);
    return true;
}

// Table 11. Recommended Initialization Sequence for SPI Interface
void MC3635_reset(SPI_Handle spiHandle)
{
    uint8_t value = 0;
    uint32_t i = 0;

    MC3635_writeReg(spiHandle, MC36XX_REG_MODE_C, 0x01);
    MC3635_writeReg(spiHandle, MC36XX_REG_RESET, 0x40);
    usleep(1000); // 1ms
    while (value == 0)
    { // look for non-zero
        value = MC3635_readReg(spiHandle, MC36XX_REG_PROD);
        i++;
        if (i > MC3635_MAX_TRY)
        {
            return;
        }
    }
    value = 0;
    while (value != 0x80)
    { // verify SPI mode
        MC3635_writeReg(spiHandle, MC36XX_REG_FEATURE_CTL, 0x80);
        value = MC3635_readReg(spiHandle, MC36XX_REG_FEATURE_CTL);
        i++;
        if (i > MC3635_MAX_TRY)
        {
            return;
        }
    }
    MC3635_writeReg(spiHandle, MC36XX_REG_PWR_CONTROL, 0x42);
    MC3635_writeReg(spiHandle, MC36XX_REG_MODE_C, 0x01);
    usleep(10000); // 10ms
    MC3635_writeReg(spiHandle, MC36XX_REG_DMX, 0x01);
    MC3635_writeReg(spiHandle, MC36XX_REG_DMY, 0x80);
    MC3635_writeReg(spiHandle, MC36XX_REG_INIT_2, 0x00);
    MC3635_writeReg(spiHandle, MC36XX_REG_INIT_3, 0x00);
}
// void MC3635_wake(void);
void MC3635_stop(SPI_Handle spiHandle)
{
    MC3635_SetMode(spiHandle, MC36XX_MODE_STANDBY);
}

bool MC3635_readRegBit(SPI_Handle spiHandle, uint8_t reg, uint8_t pos)
{
    uint8_t value;
    value = MC3635_readReg(spiHandle, reg);
    return ((value >> pos) & 1);
}
uint8_t MC3635_readReg(SPI_Handle spiHandle, uint8_t reg)
{
    SPI_Transaction transaction;
    bool transferOK;
    unsigned char rxBuffer[SPI_MSG_LENGTH];
    unsigned char txBuffer[SPI_MSG_LENGTH];

    txBuffer[0] = MC3635_ADDR_BIT | MC3635_READ_CMD | reg;
    txBuffer[1] = 0x00; // read location
    transaction.count = SPI_MSG_LENGTH;
    transaction.txBuf = (void*) txBuffer;
    transaction.rxBuf = (void*) rxBuffer;
    transferOK = SPI_transfer(spiHandle, &transaction);

    if (transferOK)
    {
        return rxBuffer[1];
    }
    return NULL;
}

void MC3635_readRegs(SPI_Handle spiHandle, uint8_t reg, uint8_t *buffer,
                     uint8_t len)
{
    SPI_Transaction transaction;
    unsigned char txBuffer[1];

    txBuffer[0] = MC3635_ADDR_BIT | MC3635_READ_CMD | reg;
    transaction.count = 1;
    transaction.txBuf = (void*) txBuffer;
    transaction.rxBuf = (void*) NULL;
    SPI_transfer(spiHandle, &transaction);

    transaction.count = len;
    transaction.txBuf = (void*) NULL;
    transaction.rxBuf = (void*) buffer;
    SPI_transfer(spiHandle, &transaction);
}

void MC3635_writeRegBit(SPI_Handle spiHandle, uint8_t reg, uint8_t pos,
bool state)
{
    uint8_t value;
    value = MC3635_readReg(spiHandle, reg);

    if (state)
    {
        value |= (1 << pos);
    }
    else
    {
        value &= ~(1 << pos);
    }

    MC3635_writeReg(spiHandle, reg, value);
}
void MC3635_writeReg(SPI_Handle spiHandle, uint8_t reg, uint8_t value)
{
    SPI_Transaction transaction;
    unsigned char txBuffer[SPI_MSG_LENGTH];

    txBuffer[0] = MC3635_ADDR_BIT | reg;
    txBuffer[1] = value;
    transaction.count = SPI_MSG_LENGTH;
    transaction.txBuf = (void*) txBuffer;
    transaction.rxBuf = (void*) NULL;
    SPI_transfer(spiHandle, &transaction);

    return;
}

void MC3635_SetMode(SPI_Handle spiHandle, MC36XX_mode_t mode)
{
    uint8_t value;
    uint8_t cfgfifovdd = 0x42;

    value = MC3635_readReg(spiHandle, MC36XX_REG_MODE_C);
    value &= 0b11110000;
    value |= mode;

    MC3635_writeReg(spiHandle, MC36XX_REG_PWR_CONTROL, cfgfifovdd);
    MC3635_writeReg(spiHandle, MC36XX_REG_MODE_C, value);
}

// Table 27. Setup Steps For Sniff Using “Rate 15”
// ultra low power (ULP) mode
void MC3635_sniff(SPI_Handle spiHandle)
{
    MC3635_SetMode(spiHandle, MC36XX_MODE_SNIFF);

    // DIY from docs?
//    MC3635_writeReg(spiHandle, MC36XX_REG_MODE_C, 0x01);
//    MC3635_writeReg(spiHandle, MC36XX_REG_RANGE_C, 0x04);
//    MC3635_writeReg(spiHandle, MC36XX_REG_FIFO_C, 0x41); // def: 0x5D
//    MC3635_writeReg(spiHandle, MC36XX_REG_PMCR, 0x30);
//    MC3635_writeReg(spiHandle, MC36XX_REG_WAKE_C, 0x10);
//    MC3635_writeReg(spiHandle, MC36XX_REG_TRIGC, 0x30);
//    MC3635_writeReg(spiHandle, MC36XX_REG_WAKE_C, 0x30);
//    MC3635_writeReg(spiHandle, MC36XX_REG_TRIGC, 0x01);
//    MC3635_writeReg(spiHandle, MC36XX_REG_WAKE_C, 0x60);
//    MC3635_writeReg(spiHandle, MC36XX_REG_TRIGC, 0x52);
//    MC3635_writeReg(spiHandle, MC36XX_REG_WAKE_C, 0x70);
//    MC3635_writeReg(spiHandle, MC36XX_REG_TRIGC, 0x01);
//    MC3635_writeReg(spiHandle, MC36XX_REG_WAKE_C, 0x0F);
//    MC3635_writeReg(spiHandle, MC36XX_REG_MODE_C, 0x02);
//
//    MC3635_writeReg(spiHandle, MC36XX_REG_SNIFFTH_C, 0x0A); // reg 0x13, X-axis, thresh=0x0A
//    MC3635_writeReg(spiHandle, MC36XX_REG_SNIFF_CONF_C, 0x01); // reg 0x14, X-axis
//    // turn on interrupt for SNIFF mode
//    MC3635_writeReg(spiHandle, MC36XX_REG_INTR_C, 0x06); // 0x06 = open drain, active LOW; pull-up on GPIO
}
void MC3635_sniffReset(SPI_Handle spiHandle)
{
    uint8_t value;

    value = MC3635_readReg(spiHandle, MC36XX_REG_SNIFF_CONF_C);
    value |= 0b10000000;

    MC3635_writeReg(spiHandle, MC36XX_REG_SNIFF_CONF_C, value);
}

void MC3635_SetSniffThreshold(SPI_Handle spiHandle, MC36XX_axis_t axis_cfg,
                              uint8_t sniff_thr)
{
    uint8_t value;
    uint8_t regSniff_addr;
    value = MC3635_readReg(spiHandle, MC36XX_REG_SNIFFTH_C);

    switch (axis_cfg)
    {
    case MC36XX_AXIS_X:
        regSniff_addr = 0x01; //Put X-axis to active
        break;
    case MC36XX_AXIS_Y: //Put Y-axis to active
        regSniff_addr = 0x02;
        break;
    case MC36XX_AXIS_Z: //Put Z-axis to active
        regSniff_addr = 0x03;
        break;
    default:
        break;
    }

    MC3635_writeReg(spiHandle, MC36XX_REG_SNIFF_CONF_C, regSniff_addr);
    value |= sniff_thr;
    MC3635_writeReg(spiHandle, MC36XX_REG_SNIFFTH_C, value);
}

//Set Sniff detect counts, 1~62 events
void MC3635_SetSniffDetectCount(SPI_Handle spiHandle, MC36XX_axis_t axis_cfg,
                                uint8_t sniff_cnt)
{
    uint8_t value;
    uint8_t sniff_cfg;
    uint8_t regSniff_addr;

    sniff_cfg = MC3635_readReg(spiHandle, MC36XX_REG_SNIFF_CONF_C);

    switch (axis_cfg)
    {
    case MC36XX_AXIS_X: //Select x detection count shadow register
        regSniff_addr = 0x05;
        break;
    case MC36XX_AXIS_Y: //Select y detection count shadow register
        regSniff_addr = 0x06;
        break;
    case MC36XX_AXIS_Z: //Select z detection count shadow register
        regSniff_addr = 0x07;
        break;
    default:
        break;
    }

    sniff_cfg |= regSniff_addr;
    MC3635_writeReg(spiHandle, MC36XX_REG_SNIFF_CONF_C, sniff_cfg);

    value = MC3635_readReg(spiHandle, MC36XX_REG_SNIFFTH_C);

    value |= sniff_cnt;
    MC3635_writeReg(spiHandle, MC36XX_REG_SNIFFTH_C, value);

    sniff_cfg |= 0x08;
    MC3635_writeReg(spiHandle, MC36XX_REG_SNIFF_CONF_C, sniff_cfg);
}

//Set sensor interrupt mode
void C3635_SetSniffAndOrN(SPI_Handle spiHandle, MC36XX_andorn_t logicandor)
{
    uint8_t value;

    value = MC3635_readReg(spiHandle, MC36XX_REG_SNIFFTH_C);

    switch (logicandor)
    {
    case MC36XX_ANDORN_OR:  //Axis or mode
        value &= 0xBF;
        break;
    case MC36XX_ANDORN_AND: //Axis and mode
        value |= 0x40;
        break;
    default:
        break;
    }

    MC3635_writeReg(spiHandle, MC36XX_REG_SNIFFTH_C, value);
}

//Set sensor sniff delta mode
void MC3635_SetSniffDeltaMode(SPI_Handle spiHandle,
                              MC36XX_delta_mode_t deltamode)
{
    uint8_t value;

    value = MC3635_readReg(spiHandle, MC36XX_REG_SNIFFTH_C);

    switch (deltamode)
    {
    case MC36XX_DELTA_MODE_C2P: //Axis C2P mode
        value &= 0x7F;
        break;
    case MC36XX_DELTA_MODE_C2B: //Axis C2B mode
        value |= 0x80;
        break;
    default:
        break;
    }

    MC3635_writeReg(spiHandle, MC36XX_REG_SNIFFTH_C, value);

    value = MC3635_readReg(spiHandle, MC36XX_REG_SNIFFTH_C);
}

void MC3635_SetINTCtrl(SPI_Handle spiHandle, uint8_t fifo_thr_int_ctl,
                       uint8_t fifo_full_int_ctl, uint8_t fifo_empty_int_ctl,
                       uint8_t acq_int_ctl, uint8_t wake_int_ctl)
{

    MC3635_SetMode(spiHandle, MC36XX_MODE_STANDBY);

    CfgINT = (((fifo_thr_int_ctl & 0x01) << 6)
            | ((fifo_full_int_ctl & 0x01) << 5)
            | ((fifo_empty_int_ctl & 0x01) << 4) | ((acq_int_ctl & 0x01) << 3)
            | ((wake_int_ctl & 0x01) << 2) | MC36XX_INTR_C_IAH_ACTIVE_LOW //MC36XX_INTR_C_IAH_ACTIVE_HIGH//
            | MC36XX_INTR_C_IPP_MODE_OPEN_DRAIN); // MC36XX_INTR_C_IPP_MODE_PUSH_PULL
    MC3635_writeReg(spiHandle, MC36XX_REG_INTR_C, CfgINT);
}

void MC3635_SetSniffAndOrN(SPI_Handle spiHandle, MC36XX_andorn_t logicandor)
{
    uint8_t value;

    value = MC3635_readReg(spiHandle, MC36XX_REG_SNIFFTH_C);

    switch (logicandor)
    {
    case MC36XX_ANDORN_OR:  //Axis or mode
        value &= 0xBF;
        break;
    case MC36XX_ANDORN_AND: //Axis and mode
        value |= 0x40;
        break;
    default:
        break;
    }

    MC3635_writeReg(spiHandle, MC36XX_REG_SNIFFTH_C, value);
}

void MC3635_INTHandler(SPI_Handle spiHandle,
                       MC36XX_interrupt_event_t *ptINT_Event)
{
    uint8_t value;

    value = MC3635_readReg(spiHandle, MC36XX_REG_STATUS_2);

    ptINT_Event->bWAKE = ((value >> 2) & 0x01);
    ptINT_Event->bACQ = ((value >> 3) & 0x01);
    ptINT_Event->bFIFO_EMPTY = ((value >> 4) & 0x01);
    ptINT_Event->bFIFO_FULL = ((value >> 5) & 0x01);
    ptINT_Event->bFIFO_THRESHOLD = ((value >> 6) & 0x01);
    ptINT_Event->bSWAKE_SNIFF = ((value >> 7) & 0x01);

    value &= 0x03;
    MC3635_writeReg(spiHandle, MC36XX_REG_STATUS_2, value);
}

MC36XX_acc_t MC3635_readRawAccel(SPI_Handle spiHandle)
{
    //{2g, 4g, 8g, 16g, 12g}
    float faRange[5] = { 19.614f, 39.228f, 78.456f, 156.912f, 117.684f };
    //{6bit, 7bit, 8bit, 10bit, 12bit, 14bit}
    float faResolution[6] = { 32.0f, 64.0f, 128.0f, 512.0f, 2048.0f, 8192.0f };

    uint8_t rawData[6]; // was byte type
    // Read the six raw data registers into data array
    MC3635_readRegs(spiHandle, MC36XX_REG_XOUT_LSB, rawData, 6);
    x = (short) ((((unsigned short) rawData[1]) << 8) | rawData[0]);
    y = (short) ((((unsigned short) rawData[3]) << 8) | rawData[2]);
    z = (short) ((((unsigned short) rawData[5]) << 8) | rawData[4]);

    AccRaw.XAxis = (short) (x);
    AccRaw.YAxis = (short) (y);
    AccRaw.ZAxis = (short) (z);
    AccRaw.XAxis_g = (float) (x) / faResolution[CfgResolution]
            * faRange[CfgRange];
    AccRaw.YAxis_g = (float) (y) / faResolution[CfgResolution]
            * faRange[CfgRange];
    AccRaw.ZAxis_g = (float) (z) / faResolution[CfgResolution]
            * faRange[CfgRange];

    return AccRaw;
}
