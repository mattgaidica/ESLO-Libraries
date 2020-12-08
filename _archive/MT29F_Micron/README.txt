Software Driver Readme File for Micron MT29F NAND device 
  
- Release Version 1.1 (Mar 2013) 
  
  nand_MT29F_lld.h  	v1.1 
  nand_MT29F_lld.c  	v1.1 
  nand_hw_if.h			v1.0 
  common.h				v1.0 
  
Note:  
 
Added OTP function: 
 
- NAND_OTP_Mode_Enter() 
- NAND_OTP_Mode_Exit() 
- NAND_OTP_Mode_Protect() 
- NAND_OTP_Page_Program() 
- NAND_OTP_Spare_Program() 
- NAND_OTP_Page_Read() 
- NAND_OTP_Spare_Read() 
 
Driver tested on the following devices: 
 
- MT29F2G08ABAFAWP 
- MT29F2G16ABAFAWP 
- MT29F8G08ABACAWP 
- MT29F8G16ABACAWP 
- MT29F4G08ABAEAWP 
- MT29F4G16ABAEAWP 
- MT29F32G08AFABA 
- MT29F1G08ABADAWP 
- MT29F4G16ABADAWP 
 
Unimplemented function: 
 
- NAND_Read_Unique_Id() 
- NAND_Cache_Read() 
- NAND_Cache_Program() 
- NAND_Multiplane_Copy_Back() 
- NAND_Multiplane_Page_Program() 
- NAND_Multiplane_Block_Erase() 
- NAND_Lock_Down() 
- NAND_Unlock_Down()