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
		HAL_Delay(500);
		load_app(result);
	}else
	{
		printf("No available application to load!\r\n");
		while(1)
		{
			HAL_GPIO_TogglePin(GPIOB,LED_RED_Pin);
			HAL_Delay(500);
		}
	}

}

void load_app(int8_t app_num)
{
	uint32_t jump_PC_ptr;
	application_info_t selected_application;

	/* Retrieving PC */
	jump_PC_ptr = *(uint32_t*)(BOOTLOADER_ADDRESS + FLASH_SECTOR_SIZE * app_num + 4);
	selected_application.func_p = (application_t)jump_PC_ptr;

	/* Retrieving SP */
	selected_application.stack_addr = *(uint32_t*)(BOOTLOADER_ADDRESS + FLASH_SECTOR_SIZE * app_num);
	__disable_irq();

    int i;
	// Disable IRQs
    for (i = 0; i < 8; i ++) NVIC->ICER[i] = 0xFFFFFFFF;
    // Clear pending IRQs
    for (i = 0; i < 8; i ++) NVIC->ICPR[i] = 0xFFFFFFFF;

    /* Jump to the Application */

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
	uint32_t memory_content = 0;
	boot_information_t boot_information;

	uint8_t status = 0;
	uint8_t crc_status = 0;

	uint8_t app_num = 0;
	uint32_t* app_start_address = 0;

	uint32_t app_size_in_byte = 131072-4;
	uint32_t crc_len_in_32 = (uint32_t)(app_size_in_byte/32);
	uint8_t crc_len_in_byte = (uint8_t)(app_size_in_byte%32);

	uint32_t crc_result = 0;
	uint32_t crc_expected = 0xF684CAE4;

	for(; app_num < NUMBER_OF_APP; app_num++)
	{
		app_start_address = BOOTLOADER_ADDRESS + FLASH_SECTOR_SIZE*(app_num+1);
		memory_content = (uint32_t)*(app_start_address);
		//if(memory_content >= APP_ADDRESS_SPACE_LOW && memory_content <= APP_ADDRESS_SPACE_MAX)
		if(memory_content != 0xffffffff)
		{
			printf("APP%d is presented.\r\n", app_num+1);
			status = status ^ (1 << (app_num+1));

			crc_result = HAL_CRC_Calculate(&hcrc, app_start_address, crc_len_in_32);
			//uint8_t remaining_content = app_start_address+app_size_in_byte-crc_len_in_byte;
			printf("Resulting CRC is '0x%X'\r\n",crc_result);

			//crc_expected = (uint32_t)(*app_start_address);
			printf("Expected CRC is '0x%X'\r\n",crc_expected);

			if(crc_result == crc_expected){
				crc_status = crc_status ^ (1 << (app_num+1));
				printf("CRC check passed for APP%d \r\n",app_num+1);

			}else{
				printf("CRC mismatch for APP%d \r\n",app_num+1);
			}

		}else{
			printf("APP%d is NOT presented.\r\n", app_num+1);
		}
	}
	boot_information.existance = status;

	boot_information.validation = crc_status;

	return boot_information;
}
