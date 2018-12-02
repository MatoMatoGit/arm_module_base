/*
 * storage_config.h
 *
 *  Created on: 24 aug. 2018
 *      Author: Dorus
 */

#ifndef STORAGE_CONFIG_H_
#define STORAGE_CONFIG_H_

#define STORAGE_CONFIG_DATA_SECTOR					126
#define STORAGE_CONFIG_DATA_SECTOR_ADDR				0x0801F800
#define STORAGE_CONFIG_DATA_SECTOR_SIZE_BYTES		0x0400

#define STORAGE_CONFIG_METADATA_SECTOR				127
#define STORAGE_CONFIG_METADATA_SECTOR_ADDR			0x0801FC00
#define STORAGE_CONFIG_METADATA_SECTOR_SIZE_BYTES	0x0400

/* File specification. */
typedef enum {
	FILE_SCHEDULE = 0,
	FILE_TIME,
	FILE_NUM /* Must be last in this enum. */
}File_t;


#define FILE_SIZE_SCHEDULE					0x200
#define FILE_SIZE_TIME						0x200
#define FILE_SIZE_LOG						0x400

#endif /* STORAGE_CONFIG_H_ */
