/*
 * bootloader.h
 *
 *  Created on: Sep 1, 2022
 *      Author: kaanari
 */

#ifndef INC_BOOTLOADER_H_
#define INC_BOOTLOADER_H_

#include "main.h"
#include "stdio.h"

extern CRC_HandleTypeDef hcrc;

/*ERROR DEFINITIONS*/
#define NO_AVAILABLE_APP -1

#define FLASH_SIZE 0x00100000

#define BOOTLOADER_ADDRESS 0x08000000
#define APP1_ADDRESS BOOTLOADER_ADDRESS + FLASH_SECTOR_SIZE
#define APP2_ADDRESS APP1_ADDRESS + FLASH_SECTOR_SIZE


#define APP_ADDRESS_SPACE_LOW BOOTLOADER_ADDRESS + FLASH_SECTOR_SIZE
#define APP_ADDRESS_SPACE_MAX BOOTLOADER_ADDRESS + FLASH_SIZE-1


#define NUMBER_OF_APP 7

#define FLASH_SECTOR_SIZE 0x00020000 /* 128 KB */
#define FLASH_BANK_SIZE 0x00020000


typedef void (*application_t)(void);

typedef struct
{
	uint8_t existance;
	uint8_t validation;
}boot_information_t;

typedef struct
{
	//uint32_t CRC_value;
	uint32_t stack_addr;
	application_t func_p;
} application_info_t;

int8_t select_application_to_boot(boot_information_t boot_information);

boot_information_t check_applications();

void load_app(int8_t app_num);

#endif /* INC_BOOTLOADER_H_ */
