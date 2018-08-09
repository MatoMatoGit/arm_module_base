/*
 * PermStorageMngr.c
 *
 *  Created on: 16 apr. 2018
 *      Author: Dorus
 */

#include "storage.h"

#include "flash.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#define STORAGE_DATA_SECTOR_ADDR_END STORAGE_CONFIG_DATA_SECTOR_ADDR + STORAGE_CONFIG_DATA_SECTOR_SIZE_BYTES
#define STORAGE_METADATA_SECTOR_ADDR_END STORAGE_CONFIG_METADATA_SECTOR_ADDR + STORAGE_CONFIG_METADATA_SECTOR_SIZE_BYTES

#define METADATA_ADDR_IS_VALID(addr) ( \
(addr <= STORAGE_METADATA_SECTOR_ADDR_END) && \
(addr >= STORAGE_CONFIG_METADATA_SECTOR_ADDR) \
)

#define DATA_ADDR_IS_VALID(addr) ( \
(addr <= STORAGE_DATA_SECTOR_ADDR_END) && \
(addr >= STORAGE_CONFIG_DATA_SECTOR_ADDR) \
)

#define FILE_ADDR_IS_VALID(addr, idx) ( \
(addr >= Metadata[idx].addr) && \
(addr <= Metadata[idx].addr + Metadata[idx].size) \
)

typedef struct {
	File_t fd;
	uintptr_t addr;
	U32_t size;
	U32_t write_offset;
	U32_t read_offset;
}FileMetadata_t;

FileMetadata_t Metadata[FILE_NUM];
uintptr_t MetadataAddress = STORAGE_CONFIG_METADATA_SECTOR_ADDR;
#define METADATA_TOTAL_SIZE_BYTES sizeof(FileMetadata_t) * FILE_NUM

static SysResult_t IMetadataStore(FileMetadata_t *metadata);
static SysResult_t IMetadataLoad(U32_t idx);
static SysResult_t IDataStore(uintptr_t addr, void *data, U32_t size);
static SysResult_t IDataLoad(U32_t idx, void *data, U32_t size);

SysResult_t StorageInit(void)
{
	SysResult_t res = SYS_RESULT_OK;

	for(U32_t i = 0; i < (U32_t)FILE_NUM; i++) {
		res = IMetadataLoad(i);
		if(res != SYS_RESULT_OK) {
			break;
		}
	}

	return res;
}

SysResult_t StorageMetadataSync(void)
{
	return IMetadataStore();
}

SysResult_t StorageFormat(U32_t f_sizes[FILE_NUM])
{
	SysResult_t res = SYS_RESULT_OK;

	if(flash_erase(STORAGE_CONFIG_DATA_SECTOR) != FLASH_RESULT_OK) {
		res = SYS_RESULT_ERROR;
	}
	if(res == SYS_RESULT_OK) {
		if(flash_erase(STORAGE_CONFIG_METADATA_SECTOR) != FLASH_RESULT_OK) {
			res = SYS_RESULT_ERROR;
		}
	}

	for(U32_t i = 0; i < (U32_t)FILE_NUM; i++) {
		res = IMetadataInit(i, f_sizes[i]);
		if(res != SYS_RESULT_OK) {
			break;
		}
	}

	return res;
}

void StorageFileReadOffsetSet(File_t fd, U32_t read_offset)
{
	Metadata[fd].read_offset = read_offset;
}

U32_t StorageFileReadOffsetGet(File_t fd)
{
	return Metadata[fd].read_offset;
}

U32_t StorageFileWriteOffsetGet(File_t fd)
{
	return Metadata[fd].write_offset;
}

SysResult_t StorageFileWrite(File_t fd, void *data, U32_t size)
{
	SysResult_t res = SYS_RESULT_INV_ARG;
	
	if(data != NULL && size > 0) {
		res = IDataStore((U32_t)fd, data, size);
	}
	
	return res;
}

SysResult_t StorageFileRead(File_t fd, void *data, U32_t size)
{
	SysResult_t res = SYS_RESULT_INV_ARG;

	if(data != NULL && size > 0) {
		res = IDataLoad((U32_t)fd, data, size, true);
	}

	return res;
}

SysResult_t StorageFileReadConsistent(File_t fd, void *data, U32_t size)
{
	SysResult_t res = SYS_RESULT_INV_ARG;

	if(data != NULL && size > 0) {
		res = IDataLoad((U32_t)fd, data, size, false);
	}

	return res;
}

static SysResult_t IMetadataInit(U32_t idx, U32_t size)
{
	SysResult_t res = SYS_RESULT_OK;

	/* If this is the first file metadata, initialize the address at the configured
	 * base address. */
	if(idx == 0) {
		Metadata[idx].addr = STORAGE_CONFIG_DATA_SECTOR_ADDR;
	} else {
		Metadata[idx].addr = Metadata[idx -1].addr + size;
	}
	/* Check if the calculated address is within bounds. */
	if(DATA_ADDR_IS_VALID(Metadata[idx].addr)) {
		Metadata[idx].addr = 0;
		res = SYS_RESULT_ERROR;
	}

	/*If the address is valid initialize the remaining fields and write the metadata to flash. */
	if(res == SYS_RESULT_OK) {
		Metadata[idx].fd = idx;
		Metadata[idx].write_offset = Metadata[idx].read_offset = 0;
		Metadata[idx].size = size;
	}

	return res;
}

static SysResult_t IMetadataStore(void)
{
	SysResult_t res = SYS_RESULT_OK;
	
	MetadataAddress += METADATA_TOTAL_SIZE_BYTES;
	if(!METADATA_ADDR_IS_VALID(MetadataAddress)) {
		res = SYS_RESULT_ERROR;
	}
	if(res == SYS_RESULT_OK) {
		if(flash_write(MetadataAddress, (void *)Metadata, METADATA_TOTAL_SIZE_BYTES) != FLASH_RESULT_OK) {
			res = SYS_RESULT_ERROR;
		}
	}

	return res;
}

static SysResult_t IMetadataLoad(void)
{
	SysResult_t res = SYS_RESULT_OK;
	U32_t i = 0;
	uintptr_t addr = MetadataAddress;

	while(i < FILE_NUM) {
		if(METADATA_ADDR_IS_VALID(addr)) {
			Metadata[i] = *((FileMetadata_t *)addr);
			addr += sizeof(FileMetadata_t);
		} else {
			res = SYS_RESULT_ERROR;
			break;
		}
	};

	return res;
}

static SysResult_t IDataStore(U32_t idx, void *data, U32_t size)
{
	SysResult_t res = SYS_RESULT_OK;
	uintptr_t addr = Metadata[idx].addr;
	
	/* Check if the address is within the storage address space. */
	if(!STORAGE_ADDR_IS_VALID(addr)) {
		res = SYS_RESULT_ERROR;
	}
	/* Check if the address is within the file address space. */
	if(res == SYS_RESULT_OK) {
		addr += Metadata[idx].write_offset;
		if(!FILE_ADDR_IS_VALID(addr, idx)) {
			res = SYS_RESULT_ERROR;
		}
	}
	/* Write the data to flash. */
	if(res == SYS_RESULT_OK) {
		if(flash_write(addr, data, size) != FLASH_RESULT_OK) {
			res = SYS_RESULT_ERROR;
		}
	}
	/* Increase the file offset. */
	if(res == SYS_RESULT_OK) {
		Metadata[idx].write_offset += size;
	}

	return res;
}

static SysResult_t IDataLoad(U32_t idx, void *data, U32_t size, bool inc_offset)
{
	SysResult_t res = SYS_RESULT_OK;
	uintptr_t addr = Metadata[idx].addr;
	
	/* Check if the address is within the storage address space. */
	if(!DATA_ADDR_IS_VALID(addr)) {
		res = SYS_RESULT_ERROR;
	}
	/* Check if the address is within the file address space. */
	if(res == SYS_RESULT_OK) {
		addr += Metadata[idx].read_offset;
		if(!FILE_ADDR_IS_VALID(addr, idx)) {
			res = SYS_RESULT_ERROR;
		}
	}
	/* Read the data from flash. */
	if(res == SYS_RESULT_OK) {
		memcpy(data, (const void *)addr, size);
	}
	/* Increase the file offset if inc_offset is true. */
	if(res == SYS_RESULT_OK && inc_offset == true) {
		Metadata[idx].read_offset += size;
	}

	return res;	
}
