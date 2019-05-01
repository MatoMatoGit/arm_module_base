/*
 * ow_common.h
 *
 *  Created on: Apr 30, 2019
 *      Author: dorus
 */

#ifndef ONEWIRE_COMMON_H_
#define ONEWIRE_COMMON_H_

// rom-code size including CRC
#define OW_ROMCODE_SIZE 8

#define OW_OK          0x00
#define OW_ERROR       0x01
#define OW_START_FAIL  0x02
#define OW_ERROR_CRC   0x03
#define OW_ERROR_BAD_ID 0x04
#define OW_BUSY          0x05

#define OW_MATCH_ROM    0x55
#define OW_SKIP_ROM     0xCC
#define OW_SEARCH_ROM   0xF0
#define OW_READ_ROM     0x33
#define OW_CONDITIONAL_SEARCH     0xEC
#define OW_OVERDRIVE_SKIP_ROM     0x3C
#define OW_OVERDRIVE_MATCH_ROM    0x69

#define OW_SHORT_CIRCUIT   0xFF
#define OW_SEARCH_FIRST    0xFF        // start new search
#define OW_PRESENCE_ERR    0x01
#define OW_DATA_ERR        0xFE
#define OW_LAST_DEVICE     0x00        // last device found
//            0x01 ... 0x40: continue searching

/**
*    @brief print the ids
*    @param  [in] id[] = rom_code
*    @param  [in] n number of id[n]
*    @param  [out] text id
*    @return OW_OK or OW_ERROR_CRC
*    @date 20/06/2011
*/
uint8_t ow_common_show_id( uint8_t id[], size_t n ,char *text);

#endif /* ONEWIRE_COMMON_H_ */
