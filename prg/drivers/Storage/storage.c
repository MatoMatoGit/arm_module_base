/*
 * storage.c
 *
 *  Created on: 16 apr. 2018
 *      Author: Dorus
 */

#include "storage.h"

#include "flash.h"
#include "Crc32.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h> /* For memcpy. */

#define STORAGE_DATA_SECTOR_ADDR_END (STORAGE_CONFIG_DATA_SECTOR_ADDR + STORAGE_CONFIG_DATA_SECTOR_SIZE_BYTES)
#define STORAGE_METADATA_SECTOR_ADDR_END (STORAGE_CONFIG_METADATA_SECTOR_ADDR + STORAGE_CONFIG_METADATA_SECTOR_SIZE_BYTES)

#define METADATA_ADDR_IS_VALID(addr) ( \
(addr < STORAGE_METADATA_SECTOR_ADDR_END) && \
(addr >= STORAGE_CONFIG_METADATA_SECTOR_ADDR) \
)

#define DATA_ADDR_IS_VALID(addr) ( \
(addr < STORAGE_DATA_SECTOR_ADDR_END) && \
(addr >= STORAGE_CONFIG_DATA_SECTOR_ADDR) \
)

#define FILE_ADDR_IS_VALID(addr, idx) ( \
(addr >= Metadata[idx].addr) && \
(addr < (Metadata[idx].addr + Metadata[idx].size)) \
)

typedef struct {
	File_t fd;
	uintptr_t addr;
	uint32_t size;
	uint32_t write_offset;
	uint32_t read_offset;
}FileMetadata_t;

FileMetadata_t Metadata[FILE_NUM];
uintptr_t MetadataAddressStore = STORAGE_CONFIG_METADATA_SECTOR_ADDR;
uintptr_t MetadataAddressLoad = STORAGE_CONFIG_METADATA_SECTOR_ADDR;

#define METADATA_TOTAL_SIZE_BYTES (sizeof(FileMetadata_t) * FILE_NUM)
#define METADATA_CRC_SIZE_BYTES sizeof(uint32_t)
#define METADATA_INC_CRC_SIZE_BYTES (METADATA_TOTAL_SIZE_BYTES + METADATA_CRC_SIZE_BYTES)

static SysResult_t IMetadataInit(uint32_t idx, uint32_t size);
static SysResult_t IMetadataStore(void);
static SysResult_t IMetadataLoad();
static uintptr_t IMetadataFind(void);
static SysResult_t IMetadataClean(void);
static uint32_t IMetadataCachedHashCalculate(void);
static uint32_t IMetadataStoredHashCalculate(uintptr_t metadata_addr);

static SysResult_t IDataStore(uint32_t idx, void *data, uint32_t size);
static SysResult_t IDataLoad(uint32_t idx, void *data, uint32_t size, bool inc_offset);

SysResult_t StorageMount(void)
{
	SysResult_t res = SYS_RESULT_OK;

	res = IMetadataLoad();

	return res;
}

SysResult_t StorageFormat(uint32_t f_sizes[FILE_NUM])
{
	SysResult_t res = SYS_RESULT_OK;

	/* Erase the data and metadata sectors. */
	if(flash_erase(STORAGE_CONFIG_DATA_SECTOR_ADDR, 1) != FLASH_RESULT_OK) {
		res = SYS_RESULT_ERROR;
	}
	if(res == SYS_RESULT_OK) {
		if(flash_erase(STORAGE_CONFIG_METADATA_SECTOR_ADDR, 1) != FLASH_RESULT_OK) {
			res = SYS_RESULT_ERROR;
		}
	}

	/* Initialize all file metadata. */
	for(uint32_t i = 0; i < (uint32_t)FILE_NUM; i++) {
		res = IMetadataInit(i, f_sizes[i]);
		if(res != SYS_RESULT_OK) {
			break;
		}
	}
	
	/* Store the initial condition metadata. */
	if(res == SYS_RESULT_OK) {
		res = IMetadataStore();
	}

	return res;
}

SysResult_t StorageFileReadOffsetSet(File_t fd, uint32_t offset, offset_opt_t opt)
{
	SysResult_t res = SYS_RESULT_FAIL;
	uint32_t curr_offset = Metadata[fd].read_offset; /* Store the current offset. */

	switch(opt) {
	case OFFSET_OPT_RELATIVE_SUB: {
		if(Metadata[fd].read_offset >= offset) { /* Make sure the subtraction does not cause underflow. */
			res = SYS_RESULT_OK;
			Metadata[fd].read_offset -= offset;
		}
		break;
	}
	case OFFSET_OPT_RELATIVE_ADD: {
		if( ((Metadata[fd].read_offset + offset) < UINT32_MAX) &&
				((Metadata[fd].read_offset + offset) <= Metadata[fd].write_offset) ) {
			res = SYS_RESULT_OK;
			Metadata[fd].read_offset += offset;
		}
		break;
	}
	default:
	case OFFSET_OPT_ABSOLUTE: {
		if(offset <= Metadata[fd].write_offset) {
			res = SYS_RESULT_OK;
			Metadata[fd].read_offset = offset;
		}
		break;
	}
	}

	if(res == SYS_RESULT_OK) {
		/* Check if the new offset results in a valid data address, if this
		 * is not the case, revert the operation. */
		if(!DATA_ADDR_IS_VALID(Metadata[fd].addr + Metadata[fd].read_offset)) {
			res = SYS_RESULT_FAIL;
			Metadata[fd].read_offset = curr_offset;
		}
	}

	return res;
}

uint32_t StorageFileReadOffsetGet(File_t fd)
{
	return Metadata[fd].read_offset;
}

uint32_t StorageFileWriteOffsetGet(File_t fd)
{
	return Metadata[fd].write_offset;
}

SysResult_t StorageFileWrite(File_t fd, void *data, uint32_t size)
{
	SysResult_t res = SYS_RESULT_INV_ARG;
	
	if(data != NULL && size > 0) {
		res = IDataStore((uint32_t)fd, data, size);
	}
	if(res == SYS_RESULT_OK) {
		res = IMetadataStore(); /* If _FAIL is returned the metadata memory is full. */
		if(res == SYS_RESULT_FAIL) {
			res = IMetadataClean(); /* Attempt to clean the metadata memory. */
		}
	}
	
	return res;
}

SysResult_t StorageFileRead(File_t fd, void *data, uint32_t size)
{
	SysResult_t res = SYS_RESULT_INV_ARG;

	if(data != NULL && size > 0) {
		res = IDataLoad((uint32_t)fd, data, size, true);
	}
	if(res == SYS_RESULT_OK) {
		res = IMetadataStore(); /* If _FAIL is returned the metadata memory is full. */
		if(res == SYS_RESULT_FAIL) {
			res = IMetadataClean(); /* Attempt to clean the metadata memory. */
		}
	}

	return res;
}

SysResult_t StorageFileReadConsistent(File_t fd, void *data, uint32_t size)
{
	SysResult_t res = SYS_RESULT_INV_ARG;

	if(data != NULL && size > 0) {
		res = IDataLoad((uint32_t)fd, data, size, false);
	}

	return res;
}

static SysResult_t IMetadataInit(uint32_t idx, uint32_t size)
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
	if(!DATA_ADDR_IS_VALID(Metadata[idx].addr)) {
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
	uint32_t metadata_crc = 0;
	uintptr_t metadata_addr = MetadataAddressStore;
	
	/* Check current address validity. */
	if(!METADATA_ADDR_IS_VALID(MetadataAddressStore + METADATA_INC_CRC_SIZE_BYTES)) {
		res = SYS_RESULT_FAIL;
	}

	/* Calculate the CRC hash over all metadata. */
	if(res == SYS_RESULT_OK) {
		metadata_crc = IMetadataCachedHashCalculate();
		if(metadata_crc == 0) {
			res = SYS_RESULT_ERROR;
		}
	}
	/* Write all metadata to flash. */
	if(res == SYS_RESULT_OK) {
		if(flash_write(MetadataAddressStore, (void *)Metadata, 
				METADATA_TOTAL_SIZE_BYTES, &MetadataAddressStore) != FLASH_RESULT_OK) {
			res = SYS_RESULT_ERROR;
		}
	}
	/* Write the CRC hash to flash. */
	if(res == SYS_RESULT_OK) {
		if(flash_write((MetadataAddressStore),
				(void *)&metadata_crc, sizeof(metadata_crc), &MetadataAddressStore) != FLASH_RESULT_OK) {
			res = SYS_RESULT_ERROR;
		}
	}

	/* Check new address validity. */
	if(!METADATA_ADDR_IS_VALID(MetadataAddressStore)) {
		res = SYS_RESULT_FAIL;
	}

	/* If all was ok, update the load address. */
	if(res == SYS_RESULT_OK) {
		MetadataAddressLoad = metadata_addr;
	}

	return res;
}

static SysResult_t IMetadataLoad(void)
{
	SysResult_t res = SYS_RESULT_FAIL;
	uint32_t i = 0;
	uintptr_t addr = IMetadataFind();
	
	/* Sanity check the metadata address. */
	if(METADATA_ADDR_IS_VALID(addr)) {
		/* Save the metadata address and load the metadata. */
		res = SYS_RESULT_OK;
		MetadataAddressLoad = addr;
		MetadataAddressStore = MetadataAddressLoad + METADATA_INC_CRC_SIZE_BYTES;
		while(i < FILE_NUM) {
			Metadata[i] = *((FileMetadata_t *)addr);
			addr += sizeof(FileMetadata_t);
			i++;
		};
	}

	return res;
}

static uintptr_t IMetadataFind(void)
{
	bool valid = true;
	uintptr_t addr = STORAGE_CONFIG_METADATA_SECTOR_ADDR;
	uintptr_t valid_addr = 0; /* Previous metadata address. */
	uint32_t metadata_crc = 0;
	uint32_t crc = 0;
	
	while( METADATA_ADDR_IS_VALID(addr) && (valid == true) ) {
		/* Calculate the CRC for the metadata. */
		crc = IMetadataStoredHashCalculate(addr);

		/* Move to where the metadata CRC would be. */
		addr += METADATA_TOTAL_SIZE_BYTES;

		/* Check the newly calculated CRC against the stored
		 * CRC. */
		metadata_crc = *((uint32_t *)addr);

		if(crc != 0 && metadata_crc == crc) {
			valid = true;
		} else {
			valid = false;
		}
		
		/* Only copy the addr if it is valid. */
		if(valid == true) {
			valid_addr = addr - METADATA_TOTAL_SIZE_BYTES;
		}
		addr+= METADATA_CRC_SIZE_BYTES;
	};
	
	return valid_addr;
}

/* Erases the metadata memory and restores the most
 * recent copy. */
static SysResult_t IMetadataClean(void)
{
	SysResult_t res = SYS_RESULT_OK;

	if(flash_erase(STORAGE_CONFIG_METADATA_SECTOR_ADDR, 1) != FLASH_RESULT_OK) {
		res = SYS_RESULT_ERROR;
	}

	if(res == SYS_RESULT_OK) {
		MetadataAddressStore = STORAGE_CONFIG_METADATA_SECTOR_ADDR;
		res = IMetadataStore();
	}

	return res;
}

static uint32_t IMetadataCachedHashCalculate(void)
{
	uint32_t crc = Crc32(0, (void *)Metadata, METADATA_TOTAL_SIZE_BYTES);
	
	if(crc == 0 || crc == UINT32_MAX) {
		crc = 0;
	}
	
	return crc;
}

static uint32_t IMetadataStoredHashCalculate(uintptr_t metadata_addr)
{
	uint32_t crc = Crc32(0, (void *)metadata_addr, METADATA_TOTAL_SIZE_BYTES);

	if(crc == 0 || crc == UINT32_MAX) {
		crc = 0;
	}

	return crc;
}

static SysResult_t IDataStore(uint32_t idx, void *data, uint32_t size)
{
	SysResult_t res = SYS_RESULT_OK;
	uintptr_t addr = Metadata[idx].addr;
	
	/* Check if the address is within the storage address space. */
	if(!DATA_ADDR_IS_VALID(addr)) {
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
		if(flash_write(addr, data, size, &addr) != FLASH_RESULT_OK) {
			res = SYS_RESULT_ERROR;
		}
	}
	/* Increase the file offset. */
	if(res == SYS_RESULT_OK) {
		Metadata[idx].write_offset += size;
	}

	return res;
}

static SysResult_t IDataLoad(uint32_t idx, void *data, uint32_t size, bool inc_offset)
{
	SysResult_t res = SYS_RESULT_OK;
	uintptr_t addr = Metadata[idx].addr;
	
	/* Check if the address is within the storage address space. */
	if(!DATA_ADDR_IS_VALID(addr)) {
		res = SYS_RESULT_ERROR;
	}
	/* Check if the requested amount of data is available. */
	if((Metadata[idx].read_offset + size) > Metadata[idx].write_offset) {
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


