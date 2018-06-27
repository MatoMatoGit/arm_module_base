/*
 * PermStorageMngr.c
 *
 *  Created on: 16 apr. 2018
 *      Author: Dorus
 */

#include <PriorRTOS.h>

#include "stm32f1xx_hal.h"
#include <stdint.h>
#include "storage.h"

typedef struct {
	U32_t size;
	U32_t offset;
	U8_t update;
}FileMetadata_t;

FileMetadata_t Metadata[FILE_NUM];

SysResult_t StorageInit(StorageConfig_t *config);

SysResult_t StorageFormat(void);

SysResult_t StorageFileFormat(File_t fd);

SysResult_t StorageFilePointerSet(File_t fd, U32_t offset);

U32_t StorageFilePointerGet(File_t fd);

SysResult_t StorageFileWrite(File_t fd);

SysResult_t StorageFileRead(File_t fd);

SysResult_t SysTaskPermStorageMngrInit(void)
{

	return SYS_RESULT_OK;
}

void SysTaskPermStorageMngr(const void *p_args, U32_t v_arg)
{

	TASK_INIT_BEGIN() {

	} TASK_INIT_END();



}
