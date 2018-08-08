/*
 * PermStorageMngr.c
 *
 *  Created on: 16 apr. 2018
 *      Author: Dorus
 */

#include "storage.h"

#include "flash.h"

#include <stdint.h>

#define STORAGE_CONFIG_SECTOR_ADDR_END STORAGE_CONFIG_SECTOR_ADDR + STORAGE_CONFIG_SECTOR_SIZE

typedef struct {
	File_t fd;
	uintptr_t addr;
	U32_t size;
	U32_t offset;
}FileMetadata_t;

FileMetadata_t Metadata[FILE_NUM];

static SysResult_t IMetadataStore(FileMetadata_t *metadata);
static SysResult_t IMetadataLoad(U32_t idx);


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

SysResult_t StorageFormat(U32_t f_sizes[FILE_NUM])
{
	SysResult_t res = SYS_RESULT_OK;

	if(flash_erase(STORAGE_CONFIG_SECTOR) != FLASH_RESULT_OK) {
		res = SYS_RESULT_ERROR;
	}

	for(U32_t i = 0; i < (U32_t)FILE_NUM; i++) {
		IMetadataInit(i, f_sizes[i]);
	}

	return res;
}

void StorageFilePointerSet(File_t fd, U32_t offset)
{
	Metadata[fd].offset = offset;
}

U32_t StorageFilePointerGet(File_t fd)
{
	return Metadata[fd].offset;
}

SysResult_t StorageFileWrite(File_t fd, void *data, U32_t size);

SysResult_t StorageFileRead(File_t fd, void *data, U32_t size);

static SysResult_t IMetadataInit(U32_t idx, U32_t size)
{
	SysResult_t res = SYS_RESULT_OK;

	/* Make space for the metadata itself. */
	size += sizeof(FileMetadata_t);

	/* If this is the first file metadata, initialize the address at the configured
	 * base address. */
	if(idx == 0) {
		Metadata[idx].addr = STORAGE_CONFIG_SECTOR_ADDR;
	} else {
		Metadata[idx].addr = Metadata[idx -1].addr + size;
	}
	/* Check if the calculated address is within bounds. */
	if(Metadata[idx].addr > STORAGE_CONFIG_SECTOR_ADDR_END) {
		Metadata[idx].addr = 0;
		res = SYS_RESULT_ERROR;
	}

	/*If the address is valid initialize the remaining fields and write the metadata to flash. */
	if(res == SYS_RESULT_OK) {
		Metadata[idx].fd = idx;
		Metadata[idx].offset = sizeof(FileMetadata_t);
		Metadata[idx].size = size;

		/* Save the metadata. */
		res = IMetadataStore(idx);
	}

	return res;
}

static SysResult_t IMetadataStore(U32_t idx)
{
	SysResult_t res = SYS_RESULT_OK;

	if(flash_write(Metadata[idx].addr, (void *)Metadata[idx], sizeof(FileMetadata_t)) != FLASH_RESULT_OK) {
		res = SYS_RESULT_ERROR;
	}

	return res;
}

static SysResult_t IMetadataLoad(U32_t idx)
{
	SysResult_t res = SYS_RESULT_OK;
	U32_t i = 0;
	FileMetadata_t tmp_md;
	uintptr_t next_addr = 0;

	tmp_md = *((FileMetadata_t *)STORAGE_CONFIG_SECTOR_ADDR);
	while(i < idx) {
		next_addr = tmp_md.addr + tmp_md.size;
		tmp_md = *((FileMetadata_t *)next_addr);
	};

	Metadata[idx] = tmp_md;

	return res;
}
