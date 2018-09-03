/*
 * storage.h
 *
 *  Created on: 16 apr. 2018
 *      Author: Dorus
 */

#ifndef STORAGE_H_
#define STORAGE_H_

#include "storage_config.h"

#include "SystemResult.h"
#include <stdint.h>

/* Mount the filesystem. A mount can only succeed if the
 * filesystem has been formatted (once). */
SysResult_t StorageMount(void);

/* Format the filesystem. The specified size will be 
 * reserved for the respective file. */
SysResult_t StorageFormat(uint32_t f_sizes[FILE_NUM]);


/* Write data to a file. */
SysResult_t StorageFileWrite(File_t fd, void *data, uint32_t size);

/* Read data from a file. */
SysResult_t StorageFileRead(File_t fd, void *data, uint32_t size);

/* Read data without moving the read offset. */
SysResult_t StorageFileReadConsistent(File_t fd, void *data, uint32_t size);


/* Set the read offset of a file. */
void StorageFileReadOffsetSet(File_t fd, uint32_t read_offset);

/* Get the read offset of a file. */
uint32_t StorageFileReadOffsetGet(File_t fd);

/* Get the write offset of a file. */
uint32_t StorageFileWriteOffsetGet(File_t fd);

#endif /* STORAGE_H_ */
