/*
 * Debug.h
 *
 *  Created on: 16 apr. 2018
 *      Author: Dorus
 */

#ifndef PERM_STORAGE_MNGR_H_
#define PERM_STORAGE_MNGR_H_

#include <OsTypes.h>
#include "SystemResult.h"

#define STORAGE_CONFIG_SECTOR_NUM			1
#define STORAGE_CONFIG_SECOTR_SIZE_BYTES	0x0400

#define STORAGE_CONFIG_STREAMS_NUM			1

struct StorageDesc;
typedef struct StorageDesc * StorageHandle_t;

SysResult_t StorageInit(void);

StorageHandle_t StorageRequest(U32_t size);

SysResult_t StorageStreamRegister(StorageHandle_t handle, Id_t rbf_stream);

SysResult_t StorageStore(StorageHandle_t handle, U8_t *data, U32_t offset, U32_t size);

SysResult_t StorageLoad(StorageHandle_t handle, U8_t *data, U32_t offset, U32_t size);

#endif /* PERM_STORAGE_MNGR_H_ */
