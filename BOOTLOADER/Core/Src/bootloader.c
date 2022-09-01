/*
 * bootloader.c
 *
 *  Created on: Sep 1, 2022
 *      Author: kaanari
 */
#include "bootloader.h"


void bootloader(void)
{

	printf("Bootloader is running...\r\n");

	boot_information_t boot_information = check_applications();



	printf("%X\r\n",boot_information.existance);
	printf("%X\r\n",boot_information.validation);

	int8_t result = select_application_to_boot(boot_information);

	if(result > 0){
		printf("APP%d is being loaded...\r\n", result);
		load_app(result);
	}else
	{
		printf("No available application to load!\r\n");
	}

}

void load_app(int8_t app_num)
{
	uint32_t jump_PC_ptr;
	application_info_t selected_application;

	/* Retrieving PC */
	jump_PC_ptr = *(uint32_t*)(BOOTLOADER_ADDRESS + FLASH_SECTOR_SIZE * app_num + 4);
	selected_application.func_p = (application_t*)jump_PC_ptr;

	/* Retrieving SP */
	selected_application.stack_addr = *(uint32_t*)(BOOTLOADER_ADDRESS + FLASH_SECTOR_SIZE * app_num);
	__set_MSP(selected_application.stack_addr);
	selected_application.func_p();
}


int8_t select_application_to_boot(boot_information_t boot_information)
{
	uint8_t app_num = 0;
	uint8_t available_apps = boot_information.existance && boot_information.validation;

	if(available_apps == 0)
	{
		return NO_AVAILABLE_APP;
	}

	while(available_apps >>= 1){
		app_num++;
	}
	app_num++;

	return app_num;
}

/* This function is used for checking if applications
 * that resides in each sector of the memory exists */
boot_information_t check_applications()
{
	boot_information_t boot_information;
	uint8_t status = 0;
	uint8_t app_num = 0;
	uint32_t* app_start_address = 0;
	for(; app_num < NUMBER_OF_APP; app_num++)
	{
		app_start_address = BOOTLOADER_ADDRESS + FLASH_SECTOR_SIZE*(app_num+1);
		if(*app_start_address >= APP_ADDRESS_SPACE_LOW && *app_start_address <= APP_ADDRESS_SPACE_MAX){
			printf("APP%d is presented.\r\n", app_num+1);
			status = status ^ (1 << (app_num+1));
		}else{
			printf("APP%d is NOT presented.\r\n", app_num+1);
		}
	}
	boot_information.existance = status;

	/* ToDo: Append CRC check here! */
	boot_information.validation = 0xFE;

	return boot_information;
}