/*
 * highly modified for MT29F2G01, by Matt Gaidica
 */

#include <MT29F.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/drivers/GPIO.h>
#include <string.h>

/* state of the driver */
uint8_t driver_status = DRIVER_STATUS_NOT_INITIALIZED;

/* global structure with device info */
struct parameter_page_t device_info;

/******************************************************************************  
 *                      Internal functions, not API  
 *****************************************************************************/

/* Address comparison constants */
#define ADDR_A_EQ_B 0   /* A = B */   
#define ADDR_A_LT_B 1   /* A < B */   
#define ADDR_A_GT_B 2   /* A > B */   

/* Internal functions */
uint16_t __as_uint16(uint8_t byte0, uint8_t byte1);
uint32_t __as_uint32(uint8_t byte0, uint8_t byte1, uint8_t byte2,
		uint8_t byte3);
void __as_string(uint8_t *src_ptr, char *dest_ptr, int start, int stop);
uint8_t __wait_for_ready(uint8_t _readyBit);
//uint8_t __is_valid_addr(nand_addr_t addr);

/******************************************************************************  
 *                      Platform functions, not API
 *****************************************************************************/

void PLATFORM_Init(uint8_t _index, uint8_t _csPin, uint8_t _csPinMirror) {
	SPI_Params spiParams;
	NAND_csPin = _csPin;
	MIRROR_csPin = _csPinMirror; // !!REMOVE
	SPI_Params_init(&spiParams);
	spiParams.frameFormat = SPI_POL0_PHA0;
	spiParams.bitRate = 1000000;
	spiParams.dataSize = 8;
//	spiParams.transferTimeout = 10; // !!what is a good value?
	spiNAND = SPI_open(_index, &spiParams);
}

void PLATFORM_Open(void) {
	GPIO_write(NAND_csPin, GPIO_CFG_OUT_LOW);
	GPIO_write(MIRROR_csPin, GPIO_CFG_OUT_LOW); // !! REMOVE
}

void PLATFORM_SendCmd(uint8_t _cmd) {
	PLATFORM_SendData(_cmd);
}

void PLATFORM_SendAddr(uint8_t _addr) {
	PLATFORM_SendData(_addr);
}

void PLATFORM_SendColumnAddr(uint8_t _addrLun, uint16_t _addrCol) {
	uint8_t txBuffer[2] = { (_addrCol << 8) | _addrLun << 4, _addrCol };
	SPI_Transaction transaction;
	bool transferOK;
	transaction.count = sizeof(txBuffer);
	transaction.txBuf = (void*) txBuffer;
	transaction.rxBuf = NULL;
	transferOK = SPI_transfer(spiNAND, &transaction);
}

void PLATFORM_SendBlockPageAddr(uint32_t _addrBlockPage) {
	uint8_t txBuffer[3] = { _addrBlockPage << 16, _addrBlockPage << 8,
			_addrBlockPage };
	SPI_Transaction transaction;
	bool transferOK;
	transaction.count = sizeof(txBuffer);
	transaction.txBuf = (void*) txBuffer;
	transaction.rxBuf = NULL;
	transferOK = SPI_transfer(spiNAND, &transaction);
}

void PLATFORM_SendData(uint8_t _data) {
	SPI_Transaction transaction;
	bool transferOK;
	transaction.count = 1;
	transaction.txBuf = &_data;
	transaction.rxBuf = NULL;
	transferOK = SPI_transfer(spiNAND, &transaction);
}

uint8_t PLATFORM_ReadData(void) {
	uint8_t rxBuffer;
	SPI_Transaction transaction;
	bool transferOK;
	transaction.count = 1;
	transaction.txBuf = (void*) NULL;
	transaction.rxBuf = &rxBuffer;
	transferOK = SPI_transfer(spiNAND, &transaction);
	return rxBuffer;
}

void PLATFORM_Wait(int microseconds) {
	Task_sleep(microseconds / Clock_tickPeriod);
}

void PLATFORM_Close(void) {
	GPIO_write(NAND_csPin, GPIO_CFG_OUT_HIGH);
	GPIO_write(MIRROR_csPin, GPIO_CFG_OUT_HIGH); //!! REMOVE
}

void PLATFORM_Toggle(void) {
	PLATFORM_Close();
	PLATFORM_Open();
}

/******************************************************************************
 *                      NAND Low Level Driver Functions  
 *****************************************************************************/

/**  
 This function initialize the driver and it must be called before
 any other function

 @return Return code
 @retval NAND_DRIVER_STATUS_INITIALIZED
 @retval DRIVER_STATUS_NOT_INITIALIZED
 */

uint8_t Init_Driver(uint8_t _index, uint8_t _csPin, uint8_t _csPinMirror) {
	/* check if the driver is previously initialized */
	if (DRIVER_STATUS_INITIALIZED == driver_status)
		return DRIVER_STATUS_INITIALIZED;

	/* initialize platform */
	PLATFORM_Init(_index, _csPin, _csPinMirror);

	/* reset at startup is mandatory */
	NAND_Reset();

	// skip ONFI check
	driver_status = DRIVER_STATUS_INITIALIZED;

	return driver_status;
}

void NAND_Unlock() {
	PLATFORM_SendCmd(CMD_SET_FEATURES); // 0x1F
	PLATFORM_SendAddr(ADDR_BLKLCK); // 0xA0
	PLATFORM_SendData(CFG_BLK_ULCK); // 0x00
}

/**  
 The RESET command must be issued to all CE#s as the first command
 after power-on.

 @return Return code
 @retval NAND_TIMEOUT
 @retval NAND_SUCCESS
 */

uint8_t NAND_Reset(void) {
	uint8_t ret;

	/* init board transfer */
	PLATFORM_Open();

	/* send command and/or address */
	PLATFORM_SendCmd(CMD_RESET);

	/* wait (see datasheet for details) */
	PLATFORM_Wait(TIME_POR);
	ret = __wait_for_ready(STATUS_OIP);

	// just unlock it here
	PLATFORM_Toggle();
	NAND_Unlock();

	/* close board transfer */
	PLATFORM_Close();

	return ret;
}

/**  
 The READ ID command is used to read identifier codes programmed into the target.
 This command is accepted by the target only when all die (LUNs) on the target are
 idle.

 @param[out] uint8_t *buffer: buffer contains device id

 @return Return code
 @retval NAND_SUCCESS
 */

uint8_t NAND_Read_ID(uint8_t *buffer) {
	int i;

	/* verify if driver is initialized */
	if (DRIVER_STATUS_INITIALIZED != driver_status)
		return DRIVER_STATUS_NOT_INITIALIZED;

	/* init board transfer */
	PLATFORM_Open();

	/* send command and/or address */
	PLATFORM_SendCmd(CMD_READID);
	PLATFORM_SendData(DUMMY_BYTE);

	/* wait (see datasheet for details) */
	PLATFORM_Wait(TIME_RD);

	/* read output */
	for (i = 0; i < NUM_OF_READID_BYTES; i++)
		buffer[i] = PLATFORM_ReadData();

	/* close board transfer */
	PLATFORM_Close();

	return NAND_SUCCESS;
}

/**  
 The READ PARAMETER PAGE command is used to read the ONFI parameter page
 programmed into the target. This command is accepted by the target only when all die
 (LUNs) on the target are idle.

 @param[out] uint8_t *ppage: a filled structure with ONFI paramter page

 @return Return code
 @retval NAND_SUCCESS
 @retval NAND_TIMEOUT

 1. 1Fh – SET FEATURES command with a feature address of B0h and data value for CFG[2:0] = 010b ( to access OTP/Parameter/UniqueID pages).

 2. 13h – PAGE READ command with a block/page address of 0x01h, and then check the status of the read completion using the GET FEATUR ES (0Fh) command with a feature address of C0h.

 3. 03h – READ FROM CACHE command with an address of 0x00h to read the data out of the NAND device (see the following Parameter Page Data Structure table for a description of the contents of the parameter page.)

 4. 1Fh – SET FEATURES command with a feature address of B0h and data value of 00h to exit the parameter page reading.
 */

uint8_t NAND_Read_Param_Page(param_page_t *ppage) {
	uint8_t rbuf[NUM_OF_PPAGE_BYTES];
	uint8_t ret;
	uint32_t i;

	/* init board transfer */
	PLATFORM_Open();

	// set feature CFG
	PLATFORM_SendCmd(CMD_SET_FEATURES); // 0x1F
	PLATFORM_SendAddr(ADDR_CFG); // 0xB0
	PLATFORM_SendData(CFG_OTP_AREA); // 0x40

	// not explicit in data sheet
	PLATFORM_Toggle();

	// read command
	PLATFORM_SendCmd(CMD_PAGE_READ); // 0x13
	PLATFORM_SendBlockPageAddr(ADDR_PARAM_PAGE); // 0x01

	PLATFORM_Wait(TIME_RD);
	ret = __wait_for_ready(STATUS_OIP); // 0x0F, 0xC0, (read & status)

	/* return if timeout */
	if (NAND_SUCCESS != ret)
		return ret;

	// read from cache @ 0x00
	PLATFORM_Toggle();
	PLATFORM_SendCmd(CMD_PAGE_READ_CACHE); // 0x03
	PLATFORM_SendColumnAddr(0x00, 0x00);
	PLATFORM_SendData(DUMMY_BYTE);

	/* read output */
	for (i = 0; i < NUM_OF_PPAGE_BYTES; i++)
		rbuf[i] = PLATFORM_ReadData(); // !! READ FROM CACHE?

	PLATFORM_Toggle();
	PLATFORM_SendCmd(CMD_SET_FEATURES);
	PLATFORM_SendAddr(ADDR_CFG);
	PLATFORM_SendData(CFG_NORM);

	/* close board transfer */
	PLATFORM_Close();

	/*
	 * Fill the parameter page data structure in the right way
	 */

	/* Parameter page signature (ONFI) */
	__as_string(rbuf, ppage->signature, 0, 3);

	/* check if the buffer contains a valid ONFI parameter page */
	if (strcmp(ppage->signature, "ONFI"))
		return NAND_BAD_PARAMETER_PAGE;

	/* Revision number */
	ppage->rev_num = __as_uint16(rbuf[4], rbuf[5]);

	/* Features supported */
	ppage->feature = __as_uint16(rbuf[6], rbuf[7]);

	/* Optional commands supported */
	ppage->command = __as_uint16(rbuf[8], rbuf[9]);

	/* Device manufacturer */
	__as_string(rbuf, ppage->manufacturer, 32, 43);

	/* Device part number */
	__as_string(rbuf, ppage->model, 44, 63);

	/* Manufacturer ID (Micron = 2Ch) */
	ppage->jedec_id = rbuf[64];

	/* Date code */
	ppage->date_code = __as_uint16(rbuf[65], rbuf[66]);

	/* Number of data bytes per page */
	ppage->data_bytes_per_page = __as_uint32(rbuf[80], rbuf[81], rbuf[82],
			rbuf[83]);

	/* Number of spare bytes per page */
	ppage->spare_bytes_per_page = __as_uint16(rbuf[84], rbuf[85]);

	/* Number of data bytes per partial page */
	ppage->data_bytes_per_partial_page = __as_uint32(rbuf[86], rbuf[87],
			rbuf[88], rbuf[89]);

	/* Number of spare bytes per partial page */
	ppage->spare_bytes_per_partial_page = __as_uint16(rbuf[90], rbuf[91]);

	/* Number of pages per block */
	ppage->pages_per_block = __as_uint32(rbuf[92], rbuf[93], rbuf[94],
			rbuf[95]);

	/* Number of blocks per unit */
	ppage->blocks_per_lun = __as_uint32(rbuf[96], rbuf[97], rbuf[98], rbuf[99]);

	/* Number of logical units (LUN) per chip enable */
	ppage->luns_per_ce = rbuf[100];

	/* Number of address cycles */
	ppage->num_addr_cycles = rbuf[101];

	/* Number of bits per cell (1 = SLC; >1= MLC) */
	ppage->bit_per_cell = rbuf[102];

	/* Bad blocks maximum per unit */
	ppage->max_bad_blocks_per_lun = __as_uint16(rbuf[103], rbuf[104]);

	/* Block endurance */
	ppage->block_endurance = __as_uint16(rbuf[105], rbuf[106]);

	/* Guaranteed valid blocks at beginning of target */
	ppage->guarenteed_valid_blocks = rbuf[107];

	/* Block endurance for guaranteed valid blocks */
	ppage->guarenteed_valid_blocks = __as_uint16(rbuf[108], rbuf[109]);

	/* Number of programs per page */
	ppage->num_programs_per_page = rbuf[110];

	/* Partial programming attributes */
	ppage->partial_prog_attr = rbuf[111];

	/* Number of bits ECC bits */
	ppage->num_ECC_bits_correctable = rbuf[112];

	/* Number of interleaved address bits */
	ppage->num_interleaved_addr_bits = rbuf[113];

	/* Interleaved operation attributes */
	ppage->interleaved_op_attr = rbuf[114];

	return ret;
}

/**  
 The SET FEATURES command writes the subfeature parameters to the
 specified feature address to enable or disable target-specific features. This command is
 accepted by the target only when all die (LUNs) on the target are idle.

 @param[in] uint8_t feature_address: address of the feature
 @param[out] uint8_t subfeature: subfeature

 @return Return code
 @retval NAND_SUCCESS
 @retval NAND_TIMEOUT
 */

uint8_t NAND_Set_Features(uint8_t feature_address, uint8_t subfeature) {
	uint8_t ret;
	ret = NAND_SUCCESS;

	/* verify if driver is initialized */
	if (DRIVER_STATUS_INITIALIZED != driver_status)
		return DRIVER_STATUS_NOT_INITIALIZED;

	/* init board transfer */
	PLATFORM_Open();

	/* send command and/or address */
	PLATFORM_SendCmd(CMD_SET_FEATURES);
	PLATFORM_SendAddr(feature_address);
	PLATFORM_SendData(subfeature); /* p0 */

	// no delay or check

	/* close board transfer */
	PLATFORM_Close();

	return ret;
}

/**  
 The GET FEATURES command reads the subfeature parameters from the
 specified feature address. This command is accepted by the target only when all die
 (LUNs) on the target are idle.

 @param[in] uint8_t feature_address: address of the feature
 @param[out] uint8_t: value of the selected feature

 @return Return code
 @retval NAND_SUCCESS
 @retval NAND_TIMEOUT
 */

uint8_t NAND_Get_Features(uint8_t feature_address, uint8_t *subfeature) {
	uint8_t ret;
	ret = NAND_SUCCESS;

	/* verify if driver is initialized */
	if (DRIVER_STATUS_INITIALIZED != driver_status)
		return DRIVER_STATUS_NOT_INITIALIZED;

	/* init board transfer */
	PLATFORM_Open();

	/* send command and/or address */
	PLATFORM_SendCmd(CMD_GET_FEATURES);
	PLATFORM_SendAddr(feature_address);
	*subfeature = PLATFORM_ReadData();

	// no delay or check

	/* close board transfer */
	PLATFORM_Close();

	return ret;
}

/**  
 Erase operations are used to clear the contents of a block in the NAND Flash array to
 prepare its pages for program operations.

 @param[in] nand_addr_t addr: any valid address within the block to erase (column address is ignored)

 @return Return code
 @retval NAND_SUCCESS
 @retval NAND_TIMEOUT
 */

//uint8_t NAND_Block_Erase(nand_addr_t addr) {
//	uint8_t row_address[5];
//	uint8_t status_reg;
//	uint8_t ret;
//
//	/* verify if driver is initialized */
//	if (DRIVER_STATUS_INITIALIZED != driver_status)
//		return DRIVER_STATUS_NOT_INITIALIZED;
//
//	/* check input parameters */
//	if (NAND_SUCCESS != __is_valid_addr(addr))
//		return NAND_INVALID_NAND_ADDRESS;
//
////	__build_cycle_addr(addr, row_address);
//
//	/* init board transfer */
//	PLATFORM_Open();
//
//	/* send command  */
//	PLATFORM_SendCmd(CMD_BLOCK_ERASE);
//
//	/* send row address (3rd, 4th, 5th cycle) */
//	PLATFORM_SendAddr(row_address[2]);
//	PLATFORM_SendAddr(row_address[3]);
//	PLATFORM_SendAddr(row_address[4]);
//
//	/* send confirm command */
////	PLATFORM_SendCmd (CMD_BLOCK_ERASE_CONFIRM);
//	/* wait */
//	PLATFORM_Wait(TIME_RD);
//	ret = __wait_for_ready(STATUS_OIP);
//
//	/* return if timeout occurs */
//	if (NAND_SUCCESS != ret)
//		return ret;
//
//	status_reg = NAND_Read_Status();
//
//	/* close board transfer */
//	PLATFORM_Close();
//
//	/* check if erase is good */
//	if (!(status_reg & STATUS_WRITE_PROTECTED))
//		return NAND_ERASE_FAILED_WRITE_PROTECT;
//
//	if (status_reg & STATUS_FAIL)
//		return NAND_ERASE_FAILED;
//
//	return ret;
//}
/**  
 The READ PAGE command copies a page from the NAND Flash array to its
 respective cache register and enables data output. This command is accepted by the die
 (LUN) when it is ready (RDY = 1, ARDY = 1).

 @return Return code
 @retval DRIVER_STATUS_NOT_INITIALIZED
 @retval NAND_INVALID_NAND_ADDRESS
 @retval NAND_READ_FAILED
 @retval NAND_SUCCESS
 @retval NAND_TIMEOUT
 */

uint8_t NAND_Page_Read(uint8_t _addrLun, uint16_t _addrCol,
		uint32_t _addrBlockPage, uint8_t *buffer, uint32_t length) {
	uint8_t ret;
	int i;

	/* verify if driver is initialized */
	if (DRIVER_STATUS_INITIALIZED != driver_status)
		return DRIVER_STATUS_NOT_INITIALIZED;

	// check addresses?

	/* x8 */
	if (length > device_info.data_bytes_per_page)
		return NAND_INVALID_LENGTH;

	/* init board transfer */
	PLATFORM_Open();

	// 02h (PROGRAM LOAD command)
	PLATFORM_SendCmd(CMD_PAGE_READ); // 0x13
	PLATFORM_SendBlockPageAddr(_addrBlockPage);

	PLATFORM_Wait(TIME_RD);
	ret = __wait_for_ready(STATUS_OIP);

	PLATFORM_Toggle();
	PLATFORM_SendCmd(CMD_PAGE_READ_CACHE); // 0x03
	PLATFORM_SendColumnAddr(_addrLun, _addrCol);
	PLATFORM_SendData(DUMMY_BYTE);

	/* get data */
	for (i = 0; i < length; i++)
		buffer[i] = PLATFORM_ReadData();

	PLATFORM_Close();

	return ret;
}

/**  
 The PROGRAM PAGE command enables the host to input data to a cache
 register, and moves the data from the cache register to the specified block and page
 address in the array of the selected die (LUN).

 @return Return code
 @retval DRIVER_STATUS_NOT_INITIALIZED
 @retval NAND_INVALID_NAND_ADDRESS
 @retval NAND_PROGRAM_FAILED_WRITE_PROTECT
 @retval NAND_PROGRAM_FAILED
 @retval NAND_SUCCESS
 @retval NAND_TIMEOUT
 */

uint8_t NAND_Page_Program(uint8_t _addrLun, uint16_t _addrCol,
		uint32_t _addrBlockPage, uint8_t *buffer, uint32_t length) {
	uint8_t ret;
	int i;

	/* verify if driver is initialized */
	if (DRIVER_STATUS_INITIALIZED != driver_status)
		return DRIVER_STATUS_NOT_INITIALIZED;

	// check addresses?

	/* x8 */
	if (length > device_info.data_bytes_per_page)
		return NAND_INVALID_LENGTH;

	/* init board transfer */
	PLATFORM_Open();

	// 06h (WRITE ENABLE command)
	PLATFORM_SendCmd(CMD_WRITE_ENABLE);

	// 02h (PROGRAM LOAD command)
	PLATFORM_Toggle();
	PLATFORM_SendCmd(CMD_PROGRAM_LOAD); // 0x02
	PLATFORM_SendColumnAddr(_addrLun, _addrCol);
	/* send data */
	for (i = 0; i < length; i++)
		PLATFORM_SendData(buffer[i]);

	// 10h (PROGRAM EXECUTE command)
	PLATFORM_Toggle();
	PLATFORM_SendCmd(CMD_PROGRAM_EXECUTE); // 0x10
	PLATFORM_SendBlockPageAddr(_addrBlockPage);

	PLATFORM_Wait(TIME_PROG);
	ret = __wait_for_ready(STATUS_P_FAIL);

	PLATFORM_Close();

	return ret;
}

/**  
 The spare read function allows to read the spare area of a page.
 Please read the datasheet for more info about ECC-on-die feature.

 @param[in] nand_addr_t addr: address where to start reading (column is ignored)
 @param[in] uint32_t length: number of byte (or word) to read
 @parma[out] uint8_t *buffer: the buffer contains the data read from the spare area

 @return Return code
 @retval DRIVER_STATUS_NOT_INITIALIZED
 @retval NAND_INVALID_NAND_ADDRESS
 @retval NAND_PROGRAM_FAILED_WRITE_PROTECT
 @retval NAND_PROGRAM_FAILED
 @retval NAND_SUCCESS
 @retval NAND_TIMEOUT
 */

//uint8_t NAND_Spare_Read(nand_addr_t addr, uint8_t *buffer, uint32_t length) {
//	uint8_t row_address[5];
//	uint8_t status_reg;
//	uint8_t ret;
//	uint32_t k;
//	int i;
//
//	/* verify if driver is initialized */
//	if (DRIVER_STATUS_INITIALIZED != driver_status)
//		return DRIVER_STATUS_NOT_INITIALIZED;
//
//	/* check input parameters */
//	if (NAND_SUCCESS != __is_valid_addr(addr))
//		return NAND_INVALID_NAND_ADDRESS;
//
//	/* x8 */
//	if (length > device_info.spare_bytes_per_page)
//		return NAND_INVALID_LENGTH;
//
//	/* spare area starts after last main area byte */
//
//	/* x8 bus width */
//	addr.column = device_info.data_bytes_per_page;
//
////	__build_cycle_addr(addr, row_address);
//
//	/* init board transfer */
//	PLATFORM_Open();
//
//	/* send command */
////	PLATFORM_SendCmd (CMD_READ_MODE);
//	/* send address */
//	for (i = 0; i < NUM_OF_ADDR_CYCLE; i++)
//		PLATFORM_SendAddr(row_address[i]);
//
//	/* return to read mode */
////	PLATFORM_SendCmd (CMD_READ_CONFIRM);
//	/* wait */
//	ret = __wait_for_ready(STATUS_OIP);
//
//	/* return if timeout */
//	if (NAND_SUCCESS != ret)
//		return ret;
//
//	/* read data */
//	for (k = 0; k < length; k++)
//		buffer[k] = PLATFORM_ReadData();
//
//	/* read status register on exit */
//	status_reg = NAND_Read_Status();
//
//	/* close board transfer */
//	PLATFORM_Close();
//
//	if (status_reg & STATUS_FAIL)
//		return NAND_READ_FAILED;
//
//	return ret;
//}
/**  
 The spare program function allows to program the spare area of a page.
 Please read the datasheet for more info about ECC-on-die feature.

 @param[in] nand_addr_t addr: address where to start reading
 @param[in] uint8_t *buffer: the buffer contains the data to program into the flash
 @param[in] uint32_t length: number of byte (or word) to program

 @return Return code
 @retval DRIVER_STATUS_NOT_INITIALIZED
 @retval NAND_INVALID_NAND_ADDRESS
 @retval NAND_PROGRAM_FAILED_WRITE_PROTECT
 @retval NAND_PROGRAM_FAILED
 @retval NAND_SUCCESS
 @retval NAND_TIMEOUT
 */

//uint8_t NAND_Spare_Program(nand_addr_t addr, uint8_t *buffer, uint32_t length) {
//	uint8_t address[5];
//	uint8_t status_reg;
//	uint32_t k;
//	int i;
//
//	/* verify if driver is initialized */
//	if (DRIVER_STATUS_INITIALIZED != driver_status)
//		return DRIVER_STATUS_NOT_INITIALIZED;
//
//	/* check input parameters */
//	if (NAND_SUCCESS != __is_valid_addr(addr))
//		return NAND_INVALID_NAND_ADDRESS;
//
//	/* x8 */
//	if (length > device_info.spare_bytes_per_page)
//		return NAND_INVALID_LENGTH;
//
//	/* x8 bus width */
//	addr.column = device_info.data_bytes_per_page;
//
////	__build_cycle_addr(addr, address);
//
//	/* init board transfer */
//	PLATFORM_Open();
//
//	/* send command */
//	PLATFORM_SendCmd(CMD_PROGRAM_LOAD);
//
//	/* send address */
//	for (i = 0; i < NUM_OF_ADDR_CYCLE; i++)
//		PLATFORM_SendAddr(address[i]);
//
//	/* send data */
//	for (k = 0; k < length; k++)
//		PLATFORM_SendData(buffer[k]);
//
//	/* send command */
////	PLATFORM_SendCmd (CMD_PROGRAM_LOAD_CONFIRM);
//	status_reg = NAND_Read_Status();
//
//	/* close board transfer */
//	PLATFORM_Close();
//
//	/* check if program is good */
//	if (!(status_reg & STATUS_WRITE_PROTECTED))
//		return NAND_PROGRAM_FAILED_WRITE_PROTECT;
//
//	if (status_reg & STATUS_FAIL)
//		return NAND_PROGRAM_FAILED;
//
//	return NAND_SUCCESS;
//}
/*  
 * NAND_Read_Unique_Id  
 */
//uint8_t NAND_Read_Unique_Id(uint8_t *buffer) {
//
//	int i;
//	uint8_t ret;
//
//	/* verify if driver is initialized */
//	if (DRIVER_STATUS_INITIALIZED != driver_status)
//		return DRIVER_STATUS_NOT_INITIALIZED;
//
//	/* init board transfer */
//	PLATFORM_Open();
//
//	/* send command and/or address */
////	PLATFORM_SendCmd (CMD_READ_UNIQ_ID);
//	PLATFORM_SendAddr(ADDR_READ_UNIQ_ID);
//
//	/* wait (see datasheet for details) */
//	PLATFORM_Wait(TIME_RD);
//
//	ret = __wait_for_ready(STATUS_OIP);
//
//	/* return if timeout */
//	if (NAND_SUCCESS != ret)
//		return ret;
//
//	/* read output */
//	for (i = 0; i < NUM_OF_UNIQUEID_BYTES; i++)
//		buffer[i] = (uint8_t) PLATFORM_ReadData();
//
//	/* close board transfer */
//	PLATFORM_Close();
//
//	return NAND_SUCCESS;
//
//}
/*  
 * NAND_Cache_Read  
 */
//uint8_t NAND_Cache_Read(void) {
//
//	/*
//	 * TO BE IMPLEMENTED
//	 *
//	 * Please contact Micron support for any request
//	 */
//
//	return NAND_UNIMPLEMENTED;
//
//}
/*  
 * NAND_Cache_Program  
 */
//uint8_t NAND_Cache_Program(void) {
//
//	/*
//	 * TO BE IMPLEMENTED
//	 *
//	 * Please contact Micron support for any request
//	 */
//
//	return NAND_UNIMPLEMENTED;
//
//}
/**  
 The LOCK command locks all of the blocks in the device. Locked blocks are write-protected
 from PROGRAM and ERASE operations.

 @return Return code
 @retval DRIVER_STATUS_NOT_INITIALIZED
 @retval NAND_SUCCESS
 */
//uint8_t NAND_Lock(void) {
//
//	/* verify if driver is initialized */
//	if (DRIVER_STATUS_INITIALIZED != driver_status)
//		return DRIVER_STATUS_NOT_INITIALIZED;
//
//	/* init board transfer */
//	PLATFORM_Open();
//
//	/* send command */
//	PLATFORM_SendCmd(CMD_LOCK);
//
//	/* close board transfer */
//	PLATFORM_Close();
//
//	return NAND_SUCCESS;
//}
/**  
 By default at power-on, if LOCK is HIGH, all the blocks are locked and protected from
 PROGRAM and ERASE operations. If portions of the device are unlocked using the UNLOCK
 (23h) command, they can be locked again using the LOCK (2Ah) command.

 @param[in] nand_addr_t start_block: address of start block to unlock (column is ignored)
 @param[in] nand_addr_t end_block: address of end block to unlock (column is ignored)

 @return Return code
 @retval DRIVER_NOT_INITIALIZED
 @retval INVALID_NAND_ADDRESS
 @retval NAND_SUCCESS
 */

//uint8_t NAND_Unlock(nand_addr_t start_block, nand_addr_t end_block) {
//	return 0x00;
//}
/**  
 The BLOCK LOCK READ STATUS (7Ah) command is used to determine the protection
 status of individual blocks.

 @param[in] nand_addr_t block_addr: address of block to check (column is ignored)

 @return Value of block status register
 @retval uint8_t block_lock_status_reg: value of block status register
 */

//uint8_t NAND_Read_Lock_Status(nand_addr_t block_addr) {
//	return 0x00;
//}
/******************************************************************************  
 *                      Internal functions, not API  
 *****************************************************************************/

/*  
 * This function is used internally from the driver in order to know when  
 * an operation (program or erase) is completed.  
 */

uint8_t __wait_for_ready(uint8_t _readyBit) {
	uint8_t ret;
	uint32_t clock_start = Clock_getTicks();

	PLATFORM_Toggle();

	/* send command and/or address */
	PLATFORM_SendCmd(CMD_GET_FEATURES); // 0x0F
	PLATFORM_SendAddr(ADDR_STATUS); // 0xC0

	while (_readyBit & PLATFORM_ReadData()
			&& (Clock_getTicks() < (clock_start + NUM_OF_TICKS_TO_TIMEOUT))) { /* do nothing */
	}

	/* check exit condition */
	if (Clock_getTicks() >= clock_start + NUM_OF_TICKS_TO_TIMEOUT)
		ret = NAND_TIMEOUT;
	else
		ret = NAND_SUCCESS;

	return ret;
}

/* NOT YET TESTED
 * Verify that an address is valid with the current  
 * device size  
 */
uint8_t __is_valid_addr(uint8_t _addrLun, uint16_t _addrCol,
		uint32_t _addrBlockPage) {
	if ((_addrCol < device_info.data_bytes_per_page)
			&& (_addrBlockPage
					< device_info.pages_per_block * device_info.blocks_per_lun)
			&& (_addrLun < device_info.luns_per_ce))
		return NAND_SUCCESS;
	return NAND_INVALID_NAND_ADDRESS;
}

// all as little-endian
uint16_t __as_uint16(uint8_t byte1, uint8_t byte0) {
	return ((uint16_t) ((byte0 << 8) | byte1));
}

uint32_t __as_uint32(uint8_t byte3, uint8_t byte2, uint8_t byte1,
		uint8_t byte0) {
	return ((uint32_t) ((byte0 << 24) | (byte1 << 16) | (byte2 << 8) | byte3));
}

void __as_string(uint8_t *src_ptr, char *dest_ptr, int start, int stop) {

	strncpy((char*) dest_ptr, (const char*) src_ptr + start, stop - start + 1);
	dest_ptr[stop - start + 1] = '\0';

}
