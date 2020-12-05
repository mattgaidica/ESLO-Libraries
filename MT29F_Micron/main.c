#include <MT29F.h>

#define MAX_BUF_SIZE 0x5000 // !! CC2652???
#define TIME_OUT_SECOND 1
#define CLOCKS_PER_SEC 100000
#define MICRON_NAND_DEFAULT
#define LITTLE_ENDIAN

int main() {
	uint8_t ret;
	param_page_t device_info;
	uint8_t write_buf[MAX_BUF_SIZE];
	uint8_t read_buf[MAX_BUF_SIZE];

	/* Lun = 0, Block = 10, Page = 10, Column = 0 */

	nand_addr_t addr = { 0, 10, 10, 0 };

	/* initialize driver */if (DRIVER_INITIALIZED == Init_Driver())

		printf("Driver initialized\n");
	else {

		printf("ERROR: Driver not initialized\n");

		return -1;
	} /* read parameter page */
	printf("Read parameter page\n");
	ret = NAND_Read_Param_Page(&device_info);
	if (NAND_SUCCESS != ret) {
		printf("NAND_Read_Param_Page FAIL! (exit code=%x)\n", ret);
		return 1;
	}
	main_area_size = device_info.data_bytes_per_page;
	spare_area_size = device_info.spare_bytes_per_page;

	/* erase block */printf("Erase\n");
	ret = NAND_Block_Erase(addr);
	if (NAND_SUCCESS != ret) {

		printf("NAND_Block_Erase FAIL! (exit code=%x)\n", ret);

		return -1;
	} /* read */
	printf("Read\n");
	ret = NAND_Page_Read(addr, read_buf, main_area_size);
	if (NAND_SUCCESS != ret) {

		printf("NAND_Page_Read FAIL! (exit code=%x)\n", ret);

		return -1;
	} /* (now block contains all 0xFF) */

	printf("%x\n", read_buf[0]);

	/* fill the write buffer with 0xBB pattern */memset(write_buf, 0xBB,
			device_info.data_bytes_per_page);

	/* program */printf("Program\n");
	ret = NAND_Page_Program(addr, write_buf, main_area_size);
	if (NAND_SUCCESS != ret) {

		printf("NAND_Page_Program FAIL! (exit code=%x)\n", ret);

		return -1;
	} /* read */
	printf("Read\n");
	ret = NAND_Page_Read(addr, read_buf, main_area_size);

	if (NAND_SUCCESS != ret) {
		printf("NAND_Page_Read FAIL! (exit code=%x)\n", ret);
		return -1;
	} /* (now page contains all 0xBB) */

	printf("%x\n", read_buf[0]);

	/* erase block before end */printf("Erase\n");
	ret = NAND_Block_Erase(addr);
	if (NAND_SUCCESS != ret) {

		printf("NAND_Block_Erase FAIL! (exit code=%x)\n", ret);

		return -1;
	}

}
