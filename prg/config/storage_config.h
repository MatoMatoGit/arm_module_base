/*
 * storage_config.h
 *
 *  Created on: 24 aug. 2018
 *      Author: Dorus
 */

#ifndef STORAGE_CONFIG_H_
#define STORAGE_CONFIG_H_

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


#endif /* STORAGE_CONFIG_H_ */
