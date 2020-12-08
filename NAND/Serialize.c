#include <Serialize.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/SPI.h>
#include <SPI_NAND.h>
#include <stdbool.h>

uint8_t NAND_csPin;
uint8_t MIRROR_csPin;
SPI_Handle spiNAND;

void NAND_Init(uint8_t _index, uint8_t _csPin, uint8_t _csPinMirror) {
	SPI_Params spiParams;
	NAND_csPin = _csPin;
	MIRROR_csPin = _csPinMirror; // !!REMOVE
	SPI_Params_init(&spiParams);
	spiParams.frameFormat = SPI_POL0_PHA0;
	spiParams.bitRate = 1000000;
	spiParams.dataSize = 8;
//	spiParams.transferTimeout = 10; // !!what is a good value?
	spiNAND = SPI_open(_index, &spiParams);

	FlashReset();
	FlashUnlock(SPI_NAND_PROTECTED_ALL_UNLOCKED);
//	FlashSetFeature(SPI_NAND_OTP_REG_ADDR, 0x00); // turn off ECC
}

/*******************************************************************************
 Function:     ConfigureSpi(SpiConfigOptions opt)
 Arguments:    opt configuration options, all acceptable values are enumerated in
 SpiMasterConfigOptions, which is a typedefed enum.
 Return Values:There is no return value for this function.
 Description:  This function can be used to properly configure the SPI master
 before and after the transfer/receive operation
 Pseudo Code:
 Step 1  : perform or skip select/deselect slave
 Step 2  : perform or skip enable/disable transfer
 Step 3  : perform or skip enable/disable receive
 *******************************************************************************/

void ConfigureSpi(SpiConfigOptions opt) {
	switch (opt) {
	case OpsWakeUp:
		// check if busy?
		GPIO_write(NAND_csPin, GPIO_CFG_OUT_LOW);
		GPIO_write(MIRROR_csPin, GPIO_CFG_OUT_LOW); // !! REMOVE
		break;
	case OpsInitTransfer:
		break;
	case OpsEndTransfer:
		GPIO_write(NAND_csPin, GPIO_CFG_OUT_HIGH);
		GPIO_write(MIRROR_csPin, GPIO_CFG_OUT_HIGH); //!! REMOVE
		break;
	default:
		break;
	}
}

/*******************************************************************************
 Function:     Serialize(const CharStream* char_stream_send,
 CharStream* char_stream_recv,
 SpiMasterConfigOptions optBefore,
 SpiMasterConfigOptions optAfter
 )
 Arguments:    char_stream_send, the char stream to be sent from the SPI master to
 the Flash memory, usually contains instruction, address, and data to be
 programmed.
 char_stream_recv, the char stream to be received from the Flash memory
 to the SPI master, usually contains data to be read from the memory.
 optBefore, configurations of the SPI master before any transfer/receive
 optAfter, configurations of the SPI after any transfer/receive
 Return Values:TRUE
 Description:  This function can be used to encapsulate a complete transfer/receive
 operation
 Pseudo Code:
 Step 1  : perform pre-transfer configuration
 Step 2  : perform transfer/ receive
 Step 3  : perform post-transfer configuration
 *******************************************************************************/
SPI_STATUS Serialize_SPI(const CharStream *char_stream_send,
		CharStream *char_stream_recv, SpiConfigOptions optBefore,
		SpiConfigOptions optAfter) {

//	uint8 *char_send, *char_recv;
//	uint16 rx_len = 0, tx_len = 0;

	SPI_Transaction transaction;
	bool transferOK;

//	tx_len = char_stream_send->length;
//	char_send = char_stream_send->pChar;

	ConfigureSpi(optBefore);

	transaction.count = char_stream_send->length;
	transaction.txBuf = char_stream_send->pChar;
	transaction.rxBuf = NULL;
	transferOK = SPI_transfer(spiNAND, &transaction);

	if (NULL_PTR != char_stream_recv) {
		transaction.count = char_stream_recv->length;
		transaction.txBuf = NULL;
		transaction.rxBuf = char_stream_recv->pChar;
		transferOK = SPI_transfer(spiNAND, &transaction);
	}

	ConfigureSpi(optAfter);

	if (transferOK)
		return RetSpiSuccess;

	return RetSpiError;
}
