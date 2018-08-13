/*
 * Debug.h
 *
 *  Created on: 16 apr. 2018
 *      Author: Dorus
 */

#ifndef STORAGE_H_
#define STORAGER_H_

#include "include/StdTypes.h"
#include "SystemResult.h"

#define STORAGE_CONFIG_DATA_SECTOR					1
#define STORAGE_CONFIG_DATA_SECTOR_ADDR				0x08010000
#define STORAGE_CONFIG_DATA_SECTOR_SIZE_BYTES		0x0400

#define STORAGE_CONFIG_METADATA_SECTOR				2
#define STORAGE_CONFIG_METADATA_SECTOR_ADDR			0x08020000
#define STORAGE_CONFIG_METADATA_SECTOR_SIZE_BYTES	0x0200

typedef enum {
	FILE_LOG = 0,
	FILE_SCHEDULE,
	FILE_NUM
}File_t;

SysResult_t StorageInit(void);

SysResult_t StorageFormat(U32_t f_sizes[FILE_NUM]);

void StorageFileReadOffsetSet(File_t fd, U32_t read_offset);

U32_t StorageFileReadOffsetGet(File_t fd);

U32_t StorageFileWriteOffsetGet(File_t fd);

SysResult_t StorageFileWrite(File_t fd, void *data, U32_t size);

SysResult_t StorageFileRead(File_t fd, void *data, U32_t size);


#endif /* STORAGE_H_ */
