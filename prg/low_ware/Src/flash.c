/*
 * flash.c
 *
 *  Created on: 27 jun. 2018
 *      Author: Dorus
 */

#include "flash.h"

#include "stm32f1xx_hal_flash.h"
#include "stm32f1xx_hal_flash_ex.h"

#define FLASH_WORD_SIZE sizeof(uint32_t)

int flash_write(uintptr_t addr_in, void *data, uint32_t size, uintptr_t *addr_out)
{
	int res = FLASH_RESULT_OK;
	uint64_t prg_data = 0;
	uint8_t byte = 0;
	uint32_t dst = (uint32_t)addr_in;
	uint32_t i = 0;
	uint32_t j = 0;

	HAL_FLASH_Unlock();
	for(i = 0; i < size; i+=FLASH_WORD_SIZE) {
		prg_data = 0;
		byte = 0;
		/* Copy available bytes. */
		for(j = 0; (j < FLASH_WORD_SIZE) && ((i + j) < size); j++) {
			byte = (*(uint8_t *)(data + i + j));
			prg_data |= (byte << (j * 8));
		}

		if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, dst, prg_data) != HAL_OK) {
			res = FLASH_RESULT_ERR;
			break;
		}

		dst+=FLASH_WORD_SIZE;
	}
	HAL_FLASH_Lock();

	if(res == FLASH_RESULT_OK && addr_out != NULL) {
		*addr_out = (uintptr_t)dst;
	}

	return res;
}

int flash_erase(uintptr_t addr, uint32_t n_pages)
{
	int res = FLASH_RESULT_ERR;
	uint32_t erase_err = 0;
	FLASH_EraseInitTypeDef erase_init = {
		.TypeErase = FLASH_TYPEERASE_PAGES,
		.Banks = 0,
		.PageAddress = (uint32_t)addr,
		.NbPages = n_pages
	};

	HAL_FLASH_Unlock();
	if(HAL_FLASHEx_Erase(&erase_init, &erase_err) == HAL_OK) {
		res = FLASH_RESULT_OK;
	}
	HAL_FLASH_Lock();

	return res;
}
//
//
//HAL_StatusTypeDef HAL_FLASH_Program(uint32_t TypeProgram, uint32_t Address, uint64_t Data);
//HAL_StatusTypeDef HAL_FLASH_Program_IT(uint32_t TypeProgram, uint32_t Address, uint64_t Data);
//
///* FLASH IRQ handler function */
//void       HAL_FLASH_IRQHandler(void);
///* Callbacks in non blocking modes */
//void       HAL_FLASH_EndOfOperationCallback(uint32_t ReturnValue);
//void       HAL_FLASH_OperationErrorCallback(uint32_t ReturnValue);
//
///**
//  * @}
//  */
//
///** @addtogroup FLASH_Exported_Functions_Group2
//  * @{
//  */
///* Peripheral Control functions ***********************************************/
//HAL_StatusTypeDef HAL_FLASH_Unlock(void);
//HAL_StatusTypeDef HAL_FLASH_Lock(void);
//HAL_StatusTypeDef HAL_FLASH_OB_Unlock(void);
//HAL_StatusTypeDef HAL_FLASH_OB_Lock(void);
//
//{
//  uint32_t TypeErase;   /*!< TypeErase: Mass erase or page erase.
//                             This parameter can be a value of @ref FLASHEx_Type_Erase */
//
//  uint32_t Banks;       /*!< Select banks to erase when Mass erase is enabled.
//                             This parameter must be a value of @ref FLASHEx_Banks */
//
//  uint32_t PageAddress; /*!< PageAdress: Initial FLASH page address to erase when mass erase is disabled
//                             This parameter must be a number between Min_Data = 0x08000000 and Max_Data = FLASH_BANKx_END
//                             (x = 1 or 2 depending on devices)*/
//
//  uint32_t NbPages;     /*!< NbPages: Number of pagess to be erased.
//                             This parameter must be a value between Min_Data = 1 and Max_Data = (max number of pages - value of initial page)*/
//
//} FLASH_EraseInitTypeDef;
//
//HAL_StatusTypeDef  HAL_FLASHEx_Erase(FLASH_EraseInitTypeDef *pEraseInit, uint32_t *PageError);
