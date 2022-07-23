#ifndef MC3635_H
#define MC3635_H

// https://github.com/mcubemems/mCube_mc36xx_arduino_driver/blob/master/MC36XX.cpp

#include <ti/drivers/SPI.h>

#define SPI_MSG_LENGTH  2
#define MC3635_READ_CMD        0x80
#define MC3635_ADDR_BIT        0x40
#define MC3635_MAX_TRY         1000

/******************************************************************************
 *** CONSTANT / DEFINE
 *****************************************************************************/
#define MC36XX_RETCODE_SUCCESS                 (0)
#define MC36XX_RETCODE_ERROR_BUS               (-1)
#define MC36XX_RETCODE_ERROR_NULL_POINTER      (-2)
#define MC36XX_RETCODE_ERROR_STATUS            (-3)
#define MC36XX_RETCODE_ERROR_SETUP             (-4)
#define MC36XX_RETCODE_ERROR_GET_DATA          (-5)
#define MC36XX_RETCODE_ERROR_IDENTIFICATION    (-6)
#define MC36XX_RETCODE_ERROR_NO_DATA           (-7)
#define MC36XX_RETCODE_ERROR_WRONG_ARGUMENT    (-8)
#define MC36XX_FIFO_DEPTH                        32
#define MC36XX_REG_MAP_SIZE                      64

/******************************************************************************
 *** CONSTANT / DEFINE
 *****************************************************************************/
#define MC36XX_INTR_C_IPP_MODE_OPEN_DRAIN    (0x00)
#define MC36XX_INTR_C_IPP_MODE_PUSH_PULL     (0x01)

#define MC36XX_INTR_C_IAH_ACTIVE_LOW         (0x00)
#define MC36XX_INTR_C_IAH_ACTIVE_HIGH        (0x02)

/******************************************************************************
 *** Register Map
 *****************************************************************************/
#define MC36XX_REG_EXT_STAT_1       (0x00)
#define MC36XX_REG_EXT_STAT_2       (0x01)
#define MC36XX_REG_XOUT_LSB         (0x02)
#define MC36XX_REG_XOUT_MSB         (0x03)
#define MC36XX_REG_YOUT_LSB         (0x04)
#define MC36XX_REG_YOUT_MSB         (0x05)
#define MC36XX_REG_ZOUT_LSB         (0x06)
#define MC36XX_REG_ZOUT_MSB         (0x07)
#define MC36XX_REG_STATUS_1         (0x08)
#define MC36XX_REG_STATUS_2         (0x09)
#define MC36XX_REG_FEATURE_CTL      (0x0D)
#define MC36XX_REG_PWR_CONTROL      (0X0F)
#define MC36XX_REG_MODE_C           (0x10)
#define MC36XX_REG_WAKE_C           (0x11)
#define MC36XX_REG_SNIFF_C          (0x12)
#define MC36XX_REG_SNIFFTH_C        (0x13)
#define MC36XX_REG_SNIFF_CONF_C     (0x14)
#define MC36XX_REG_RANGE_C          (0x15)
#define MC36XX_REG_FIFO_C           (0x16)
#define MC36XX_REG_INTR_C           (0x17)
#define MC36XX_REG_PROD             (0x18)
#define MC36XX_REG_INIT_3           (0x1A)
#define MC36XX_REG_SCRATCH          (0x1B)
#define MC36XX_REG_PMCR             (0x1C)
#define MC36XX_REG_DMX              (0x20)
#define MC36XX_REG_DMY              (0x21)
#define MC36XX_REG_GAIN             (0x21)
#define MC36XX_REG_DMZ              (0x22)
#define MC36XX_REG_RESET            (0x24)
#define MC36XX_REG_INIT_2           (0x28)
#define MC36XX_REG_TRIGC            (0x29)
#define MC36XX_REG_XOFFL            (0x2A)
#define MC36XX_REG_XOFFH            (0x2B)
#define MC36XX_REG_YOFFL            (0x2C)
#define MC36XX_REG_YOFFH            (0x2D)
#define MC36XX_REG_ZOFFL            (0x2E)
#define MC36XX_REG_ZOFFH            (0x2F)
#define MC36XX_REG_XGAIN            (0x30)
#define MC36XX_REG_YGAIN            (0x31)
#define MC36XX_REG_ZGAIN            (0x32)
#define MC36XX_REG_OPT              (0x3B)
#define MC36XX_REG_LOC_X            (0x3C)
#define MC36XX_REG_LOC_Y            (0x3D)
#define MC36XX_REG_LOT_dAOFSZ       (0x3E)
#define MC36XX_REG_WAF_LOT          (0x3F)

#define MC36XX_NULL_ADDR            (0)

typedef struct
{
    short XAxis;
    short YAxis;
    short ZAxis;
    float XAxis_g;
    float YAxis_g;
    float ZAxis_g;
} MC36XX_acc_t;

typedef enum
{
    MC36XX_GAIN_DEFAULT = 0b00,
    MC36XX_GAIN_4X = 0b01,
    MC36XX_GAIN_1X = 0b10,
    MC36XX_GAIN_NOT_USED = 0b11,
} MC36XX_gain_t;

typedef enum
{
    MC36XX_MODE_SLEEP = 0b000,
    MC36XX_MODE_STANDBY = 0b001,
    MC36XX_MODE_SNIFF = 0b010,
    MC36XX_MODE_CWAKE = 0b101,
    MC36XX_MODE_TRIG = 0b111,
} MC36XX_mode_t;

typedef enum
{
    MC36XX_RANGE_2G = 0b000,
    MC36XX_RANGE_4G = 0b001,
    MC36XX_RANGE_8G = 0b010,
    MC36XX_RANGE_16G = 0b011,
    MC36XX_RANGE_12G = 0b100,
    MC36XX_RANGE_END,
} MC36XX_range_t;

typedef enum
{
    MC36XX_RESOLUTION_6BIT = 0b000,
    MC36XX_RESOLUTION_7BIT = 0b001,
    MC36XX_RESOLUTION_8BIT = 0b010,
    MC36XX_RESOLUTION_10BIT = 0b011,
    MC36XX_RESOLUTION_12BIT = 0b100,
    MC36XX_RESOLUTION_14BIT = 0b101,  //(Do not select if FIFO enabled)
    MC36XX_RESOLUTION_END,
} MC36XX_resolution_t;

typedef enum
{
    MC36XX_CWAKE_SR_DEFAULT_54Hz = 0b0000,
    MC36XX_CWAKE_SR_14Hz = 0b0101,
    MC36XX_CWAKE_SR_28Hz = 0b0110,
    MC36XX_CWAKE_SR_54Hz = 0b0111,
    MC36XX_CWAKE_SR_105Hz = 0b1000,
    MC36XX_CWAKE_SR_210Hz = 0b1001,
    MC36XX_CWAKE_SR_400Hz = 0b1010,
    MC36XX_CWAKE_SR_600Hz = 0b1011,
    MC36XX_CWAKE_SR_END,
} MC36XX_cwake_sr_t;

typedef enum
{
    MC36XX_SNIFF_SR_DEFAULT_7Hz = 0b0000,
    MC36XX_SNIFF_SR_0p4Hz = 0b0001,
    MC36XX_SNIFF_SR_0p8Hz = 0b0010,
    MC36XX_SNIFF_SR_1p5Hz = 0b0011,
    MC36XX_SNIFF_SR_7Hz = 0b0100,
    MC36XX_SNIFF_SR_14Hz = 0b0101,
    MC36XX_SNIFF_SR_28Hz = 0b0110,
    MC36XX_SNIFF_SR_54Hz = 0b0111,
    MC36XX_SNIFF_SR_105Hz = 0b1000,
    MC36XX_SNIFF_SR_210Hz = 0b1001,
    MC36XX_SNIFF_SR_400Hz = 0b1010,
    MC36XX_SNIFF_SR_600Hz = 0b1011,
    MC36XX_SNIFF_SR_END,
} MC36XX_sniff_sr_t;

typedef enum
{
    MC36XX_FIFO_CTL_DISABLE = 0, MC36XX_FIFO_CTL_ENABLE, MC36XX_FIFO_CTL_END,
} MC36XX_fifo_ctl_t;

typedef enum
{
    MC36XX_FIFO_MODE_NORMAL = 0,
    MC36XX_FIFO_MODE_WATERMARK,
    MC36XX_FIFO_MODE_END,
} MC36XX_fifo_mode_t;

typedef enum
{
    MC36XX_ANDORN_OR = 0, MC36XX_ANDORN_AND, MC36XX_ANDORN_END,
} MC36XX_andorn_t;

typedef enum
{
    //Compare to previous
    MC36XX_DELTA_MODE_C2P = 0,
    //Compare to baseline
    MC36XX_DELTA_MODE_C2B,
    MC36XX_DELTA_MODE_END,
} MC36XX_delta_mode_t;

typedef struct
{
    unsigned char bWAKE;
    unsigned char bACQ;
    unsigned char bFIFO_EMPTY;
    unsigned char bFIFO_FULL;
    unsigned char bFIFO_THRESHOLD;
    unsigned char bRESV;
    unsigned char bSWAKE_SNIFF;
    unsigned char baPadding[2];
} MC36XX_interrupt_event_t;

typedef enum
{
    MC36XX_AXIS_X = 0, MC36XX_AXIS_Y, MC36XX_AXIS_Z, MC36XX_AXIS_END,
} MC36XX_axis_t;

typedef struct
{
    // Sensor wakes from sniff mode.
    unsigned char bWAKE;
    // New sample is ready and acquired.
    unsigned char bACQ;
    // FIFO is empty.
    unsigned char bFIFO_EMPTY;
    // FIFO is full.
    unsigned char bFIFO_FULL;
    // FIFO sample count is equal to or greater than the threshold count.
    unsigned char bFIFO_THRESHOLD;
    // Reserved
    unsigned char bRESV;
    unsigned char baPadding[2];
} MC36XX_InterruptEvent;

SPI_Handle MC3635_init(uint_least8_t CONFIG_SPI);
bool MC3635_start(SPI_Handle spiHandle);

// void MC3635_wake(void);
void MC3635_stop(SPI_Handle spiHandle);
void MC3635_reset(SPI_Handle spiHandle);
void MC3635_sniff(SPI_Handle spiHandle);
void MC3635_sniffReset(SPI_Handle spiHandle);

bool MC3635_readRegBit(SPI_Handle spiHandle, uint8_t reg, uint8_t pos);
uint8_t MC3635_readReg(SPI_Handle spiHandle, uint8_t reg);
void MC3635_readRegs(SPI_Handle spiHandle, uint8_t reg, uint8_t *buffer, uint8_t len);

void MC3635_writeRegBit(SPI_Handle spiHandle, uint8_t reg, uint8_t pos,
                        bool state);
void MC3635_writeReg(SPI_Handle spiHandle, uint8_t reg, uint8_t value);
void MC3635_SetMode(SPI_Handle spiHandle, MC36XX_mode_t mode);
void MC3635_SetSniffThreshold(SPI_Handle spiHandle, MC36XX_axis_t axis_cfg,
                              uint8_t sniff_thr);
void MC3635_SetSniffDetectCount(SPI_Handle spiHandle, MC36XX_axis_t axis_cfg,
                                uint8_t sniff_cnt);
void MC3635_SetSniffDeltaMode(SPI_Handle spiHandle,
                             MC36XX_delta_mode_t deltamode);
void MC3635_SetINTCtrl(SPI_Handle spiHandle, uint8_t fifo_thr_int_ctl,
                       uint8_t fifo_full_int_ctl, uint8_t fifo_empty_int_ctl,
                       uint8_t acq_int_ctl, uint8_t wake_int_ctl);
void MC3635_SetSniffAndOrN(SPI_Handle spiHandle, MC36XX_andorn_t logicandor);
void MC3635_INTHandler(SPI_Handle spiHandle, MC36XX_interrupt_event_t *ptINT_Event);
MC36XX_acc_t MC3635_readRawAccel(SPI_Handle spiHandle);

short x, y, z;
MC36XX_acc_t AccRaw;

#endif /*MC3635_H */
