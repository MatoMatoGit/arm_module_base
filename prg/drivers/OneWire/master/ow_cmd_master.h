/*
 * ow_cmd_master.h
 *
 *  Created on: May 1, 2019
 *      Author: dorus
 */

#ifndef ONEWIRE_CMD_MASTER_H_
#define ONEWIRE_CMD_MASTER_H_

#include "ow_common.h"

#define MAX_DEVICES 16

/**
*    @brief write command
*    @param  [in] command
*    @param  [in] id romcode, use NULL to
*    broadcast the command.
*    @date 20/06/2011
*/
uint8_t ow_cmd_master_send( uint8_t command, uint8_t id[] );

/**
*    @brief search romcode
*    @param  [in] uint8_t diff
*    @param  [out] id romcode
*    @return next_diff or OW_LAST_DEVICE or OW_DATA_ERR or OW_PRESENCE_ERR
*    @date 20/06/2011
*/
uint8_t ow_cmd_master_rom_search( uint8_t diff, uint8_t id[] );

/**
*    @brief find a device with specified family code on the 1-Wire-Bus
*    @param  [in] family_code familiy code to find
*    @param  [in/out] diff is the result of the last rom-search
*    @param  [out] is the rom-code of the sensor found
*    @return  OW_OK or OW_ERROR
*/
uint8_t ow_cmd_master_find_device(uint8_t family_code, uint8_t *diff, uint8_t id[]);

/**
*    @brief search devices with specified family code on the 1-Wire-Bus
*    @param  [in] family_code family code to search for
*    @param  [out] n number of device onewire
*    @param  [out] ids[][] array of id romcodes
*    @return OW_OK or OW_PRESENCE_ERR or OW_DATA_ERR
*    @date 20/06/2011
*/
uint8_t ow_cmd_master_search_devices(uint8_t family_code, uint8_t *n ,uint8_t *ids);

#endif /* ONEWIRE_CMD_MASTER_H_ */
