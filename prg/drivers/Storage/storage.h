/*
 * storage.h
 *
 *  Created on: 16 apr. 2018
 *      Author: Dorus
 */

#ifndef STORAGE_H_
#define STORAGER_H_

#include "include/StdTypes.h"
#include "SystemResult.h"

#define STORAGE_CONFIG_DATA_SECTOR					30
#define STORAGE_CONFIG_DATA_SECTOR_ADDR				0x08007800
#define STORAGE_CONFIG_DATA_SECTOR_SIZE_BYTES		0x0400

#define STORAGE_CONFIG_METADATA_SECTOR				31
#define STORAGE_CONFIG_METADATA_SECTOR_ADDR			0x08007C00
#define STORAGE_CONFIG_METADATA_SECTOR_SIZE_BYTES	0x0400


/* File specification. */
typedef enum {
	FILE_LOG = 0,
	FILE_SCHEDULE,
	
	FILE_NUM /* Must be last in this enum. */
}File_t;

/* Mount the filesystem. A mount can only succeed if the
 * filesystem has been formatted (once). */
SysResult_t StorageMount(void);

/* Format the filesystem. The specified size will be 
 * reserved for the respective file. */
SysResult_t StorageFormat(U32_t f_sizes[FILE_NUM]);


/* Write data to a file. */
SysResult_t StorageFileWrite(File_t fd, void *data, U32_t size);

/* Read data from a file. */
SysResult_t StorageFileRead(File_t fd, void *data, U32_t size);

/* Read data without moving the read offset. */
SysResult_t StorageFileReadConsistent(File_t fd, void *data, U32_t size);


/* Set the read offset of a file. */
void StorageFileReadOffsetSet(File_t fd, U32_t read_offset);

/* Get the read offset of a file. */
U32_t StorageFileReadOffsetGet(File_t fd);

/* Get the write offset of a file. */
U32_t StorageFileWriteOffsetGet(File_t fd);

#endif /* STORAGE_H_ */
