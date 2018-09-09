/*
 * flash.h
 *
 *  Created on: 27 jun. 2018
 *      Author: Dorus
 */

#ifndef FLASH_H_
#define FLASH_H_

#include <stdint.h>

#define FLASH_RESULT_OK		0
#define FLASH_RESULT_ERR	-1

int flash_write(uintptr_t addr_in, void *data, uint32_t size, uintptr_t *addr_out);

int flash_erase(uintptr_t addr, uint32_t n_pages);

#endif /* FLASH_H_ */
