/*
 * Debug.h
 *
 *  Created on: 16 apr. 2018
 *      Author: Dorus
 */

#ifndef PERM_STORAGE_MNGR_H_
#define PERM_STORAGE_MNGR_H_

#include <Types.h>

#define PERM_STORAGE_USER_DATA_SIZE_BYTES	0x00000400
#define PERM_STORAGE_LOG_SIZE_BYTES			0x00000400

Id_t TidPermStorageMngr;
void SysTaskPermStorageMngr(const void *p_args, U32_t v_arg);

struct PermStorageDesc;
typedef struct PermStorageDesc PermStorageHandle_t; 

PermStorageHandle_t PermStorageMngrStorageRequest(U32_t size);

S8_t PermStorageMngrStreamRegister(PermStorageHandle_t handle, Id_t stream_ringbuf);

S8_t PermStorageMngrStore(PermStorageHandle_t handle, U8_t *data, U32_t offset, U32_t size);

S8_t PermStorageMngrLoad(PermStorageHandle_t handle, U8_t *data, U32_t offset, U32_t size);

#endif /* PERM_STORAGE_MNGR_H_ */
