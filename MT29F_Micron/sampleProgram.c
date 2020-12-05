#include "common.h"
#include "nand_MT29F_lld.h"

#define MAX_BUF_SIZE 0x5000

int main() {
	MT_uint8 ret;

	param_page_t device_info;
	flash_width write_buf[MAX_BUF_SIZE];
	flash_width read_buf[MAX_BUF_SIZE];

	int main_area_size = 0, spare_area_size = 0;

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
