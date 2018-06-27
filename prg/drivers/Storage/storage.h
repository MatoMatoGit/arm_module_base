/*
 * Debug.h
 *
 *  Created on: 16 apr. 2018
 *      Author: Dorus
 */

#ifndef STORAGE_H_
#define STORAGER_H_

#include <OsTypes.h>
#include "SystemResult.h"

#define STORAGE_CONFIG_SECTOR_NUM			1
#define STORAGE_CONFIG_SECOTR_SIZE_BYTES	0x0400

typedef enum {
	FILE_LOG = 0,
	FILE_SCHEDULE,
	FILE_NUM
}File_t;

typedef struct {
	U32_t f_sizes[FILE_NUM];
}StorageConfig_t;

SysResult_t StorageInit(StorageConfig_t *config);

SysResult_t StorageFormat(void);

SysResult_t StorageFileFormat(File_t fd);

SysResult_t StorageFilePointerSet(File_t fd, U32_t offset);

U32_t StorageFilePointerGet(File_t fd);

SysResult_t StorageFileWrite(File_t fd);

SysResult_t StorageFileRead(File_t fd);


#endif /* STORAGE_H_ */
