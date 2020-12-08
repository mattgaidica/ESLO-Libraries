#include <ti/drivers/SPI.h>
#include <unistd.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define NUM_OF_TICKS_TO_TIMEOUT 10000

uint8_t NAND_csPin;
uint8_t MIRROR_csPin;
SPI_Handle spiNAND;

/** NAND address */
//typedef struct nand_address_t {
//
//	/* LUN */
//	uint32_t lun;
//
//	/* block address */
//	uint32_t block;
//
//	/* page address */
//	uint32_t page;
//
//	/* column address */
//	uint32_t column;
//
//} nand_addr_t;
/** Parameter Page Data Structure */
typedef struct parameter_page_t {
	/** Parameter page signature (ONFI) */
	char signature[5];

	/** Revision number */
	uint16_t rev_num;

	/** Features supported */
	uint16_t feature;

	/** Optional commands supported */
	uint16_t command;

	/** Device manufacturer */
	char manufacturer[13];

	/** Device part number */
	char model[21];

	/** Manufacturer ID (Micron = 2Ch) */
	uint8_t jedec_id;

	/** Date code */
	uint16_t date_code;

	/** Number of data bytes per page */
	uint32_t data_bytes_per_page;

	/** Number of spare bytes per page */
	uint16_t spare_bytes_per_page;

	/** Number of data bytes per partial page */
	uint32_t data_bytes_per_partial_page;

	/** Number of spare bytes per partial page */
	uint16_t spare_bytes_per_partial_page;

	/** Number of pages per block */
	uint32_t pages_per_block;

	/** Number of blocks per unit */
	uint32_t blocks_per_lun;

	/** Number of logical units (LUN) per chip enable */
	uint8_t luns_per_ce;

	/** Number of address cycles */
	uint8_t num_addr_cycles;

	/** Number of bits per cell (1 = SLC; >1= MLC) */
	uint8_t bit_per_cell;

	/** Bad blocks maximum per unit */
	uint16_t max_bad_blocks_per_lun;

	/** Block endurance */
	uint16_t block_endurance;

	/** Guaranteed valid blocks at beginning of target */
	uint8_t guarenteed_valid_blocks;

	/** Block endurance for guaranteed valid blocks */
	uint16_t block_endurance_guarenteed_valid;

	/** Number of programs per page */
	uint8_t num_programs_per_page;

	/** Partial programming attributes */
	uint8_t partial_prog_attr;

	/** Number of bits ECC bits */
	uint8_t num_ECC_bits_correctable;

	/** Number of interleaved address bits */
	uint8_t num_interleaved_addr_bits;

	/** Interleaved operation attributes */
	uint8_t interleaved_op_attr;

} param_page_t;

/* 
 * NAND Command set 
 */
#define CMD_RESET							0xFF
#define CMD_SET_FEATURES					0x1F
#define CMD_GET_FEATURES					0x0F
#define CMD_READID							0x9F
#define CMD_PAGE_READ						0x13
#define CMD_PAGE_READ_CACHE_RANDOM			0x30
#define CMD_PAGE_READ_CACHE_LAST			0x3F
#define CMD_PAGE_READ_CACHE					0x03
#define CMD_WRITE_ENABLE					0x06
#define CMD_WRITE_DISABLE					0x04
#define CMD_BLOCK_ERASE						0xD8
#define CMD_PROGRAM_EXECUTE					0x10
#define CMD_PROGRAM_LOAD					0x02
#define CMD_PROGRAM_LOAD_RANDOM				0x84
#define CMD_LOCK							0x2C

/* 
 * Significant addresses 
 */
#define ADDR_READ_ID						0x00
#define ADDR_READ_UNIQ_ID					0x00
#define ADDR_PARAM_PAGE						0x01
#define ADDR_BLKLCK							0xA0
#define ADDR_CFG							0xB0
#define ADDR_STATUS							0xC0
#define ADDR_DIESEL							0xD0

/** 
 * Significant constants 
 */
#define NUM_OF_READID_BYTES					2
#define NUM_OF_READUNIQUEID_BYTES			16
#define NUM_OF_PPAGE_BYTES					128
#define NUM_OF_UNIQUEID_BYTES				32

/* 
 * Time constants, in us (see datasheet)
 */
#define TIME_ERS							10000
#define TIME_PROG							220
#define TIME_RD								80
#define TIME_RCBSY							40
#define TIME_POR							2000
#define TIME_RST_READ						85
#define TIME_RST_PROGRAM					90
#define TIME_RST_ERASE						580

/* 
 * Driver status constants 
 */

/* normal driver state */
#define DRIVER_STATUS_INITIALIZED						0 

/* driver is not initialized */
#define DRIVER_STATUS_NOT_INITIALIZED					1 

/* 
 * Status register definition 
 */
#define	STATUS_OIP							0x00
#define STATUS_WEL							0x01
#define STATUS_E_FAIL						0x02
#define STATUS_P_FAIL						0x03
#define STATUS_ECCS0						0x04
#define STATUS_ECCS1						0x05
#define STATUS_ECCS2						0x06
#define STATUS_CRBSY						0x07

// not sure where to put these
#define DUMMY_BYTE		0x00
#define LOT_EN 			0x00
#define EEC_EN 			0x00
// ^since these are 0x00, CFG below don't need to be AND'ed
#define CFG_NORM 		0x00
#define CFG_OTP_AREA 	0x40
#define CFG_OTP_LOCK	0xC0
#define CFG_SPI_NOR		0x82
#define CFG_BLK_LCK		0xC2
#define CFG_BLK_ULCK	0x00

/* 
 * Function return constants 
 */
#define NAND_SUCCESS							0x00 
#define	NAND_GENERIC_FAIL						0x10 
#define NAND_BAD_PARAMETER_PAGE					0x20 
#define	NAND_INVALID_NAND_ADDRESS				0x30 
#define NAND_INVALID_LENGTH						0x31
#define NAND_ERASE_FAILED						0x40 
#define NAND_ERASE_FAILED_WRITE_PROTECT			0x41 
#define NAND_PROGRAM_FAILED						0x50 
#define NAND_PROGRAM_FAILED_WRITE_PROTECT		0x51 
#define NAND_READ_FAILED						0x60 
#define NAND_UNSUPPORTED						0xFD
#define NAND_TIMEOUT							0xFE

#define NAND_UNIMPLEMENTED						0xFF 

/****************************************************************************** 
 *									List of APIs 
 *****************************************************************************/

uint8_t Init_Driver(uint8_t _index, uint8_t _csPin, uint8_t _csPinMirror);

/* reset operations */
uint8_t NAND_Reset(void);

/* identification operations */
uint8_t NAND_Read_ID(uint8_t *buffer);
uint8_t NAND_Read_Param_Page(param_page_t *ppage);

/* feature operations */
uint8_t NAND_Get_Features(uint8_t feature_address, uint8_t *subfeature);
uint8_t NAND_Set_Features(uint8_t feature_address, uint8_t subfeature);

/* status operations */
//uint8_t NAND_Read_Status();
/* read operations */
uint8_t NAND_Page_Read(uint8_t _addrLun, uint16_t _addrCol,
		uint32_t _addrBlockPage, uint8_t *buffer, uint32_t length);
//uint8_t NAND_Spare_Read(nand_addr_t addr, uint8_t *buffer, uint32_t length);

/* erase operations */
//uint8_t NAND_Block_Erase(nand_addr_t addr);
/* program operations */
uint8_t NAND_Page_Program(uint8_t _addrLun, uint16_t _addrCol,
		uint32_t _addrBlockPage, uint8_t *buffer, uint32_t length);
uint8_t NAND_Random_Data_Program(uint8_t _addrLun, uint16_t _addrCol,
		uint32_t _addrBlockPage, uint8_t *buffer, uint32_t length);
//uint8_t NAND_Spare_Program(nand_addr_t addr, uint8_t *buffer, uint32_t length);

/* block lock operations */
//uint8_t NAND_Lock(void);
//void NAND_Unlock(); // included in RESET
//uint8_t NAND_Read_Lock_Status(nand_addr_t block_addr);
/****************************************************************************** 
 *							List of unimplemented APIs 
 *****************************************************************************/

/* identification operations */
//uint8_t NAND_Read_Unique_Id(uint8_t *buffer);
/* cached operations */
//uint8_t NAND_Cache_Read(void);
//uint8_t NAND_Cache_Program(void);
void PLATFORM_Init(uint8_t _index, uint8_t _csPin, uint8_t _csPinMirror);

void PLATFORM_Open(void);

void PLATFORM_SendCmd(uint8_t _cmd);

void PLATFORM_SendColumnAddr(uint8_t _addrLun, uint16_t _addrCol);
void PLATFORM_SendBlockPageAddr(uint32_t _addrBlockPage);
void PLATFORM_SendAddr(uint8_t _addr);

void PLATFORM_SendData(uint8_t data);

uint8_t PLATFORM_ReadData(void);

void PLATFORM_Wait(int microseconds); // !! Task_sleep(n/1000)

void PLATFORM_Close(void);
