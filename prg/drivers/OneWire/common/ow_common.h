/*
 * ow_common.h
 *
 *  Created on: Apr 30, 2019
 *      Author: dorus
 */

#ifndef ONEWIRE_COMMON_H_
#define ONEWIRE_COMMON_H_

#include <stdint.h>
#include <stdlib.h>

// rom-code size including CRC
#define OW_ROMCODE_SIZE 8
#define OW_CMD_SIZE		8

#define OW_OK          0x00
#define OW_ERROR       0x01
#define OW_START_FAIL  0x02
#define OW_ERROR_CRC   0x03
#define OW_ERROR_BAD_ID 0x04
#define OW_BUSY         0x05
#define OW_INV_ARG		0x06

#define OW_TR_MIN					1000	/* T Reset min. */
#define OW_TR_MAX					1100	/* T Reset max. */
#define OW_TP						200		/* T Presence */
#define OW_TO_MIN					50		/* T One min. */
#define OW_TO_MAX					70		/* T One max. */
#define OW_TZ_MIN					250		/* T Zero min. */
#define OW_TZ_MAX					300		/* T Zero max. */
#define OW_TS						50		/* T Sample */
#define OW_TRC						50		/* T Recover */
#define OW_DTOZ_MIN					OW_TZ_MIN - OW_TO_MIN		/* Delta-T One-Zero min. */
#define OW_DTOZ_MAX					OW_TZ_MAX - OW_TO_MAX		/* Delta-T One-Zero max. */
#define OW_DTZR_MIN					OW_TR_MIN - OW_DTOZ_MIN		/* Delta-T Zero-Reset min. */		
#define OW_DTZR_MAX					OW_TR_MAX - OW_DTOZ_MAX		/* Delta-T Zero-Reset max. */	

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

typedef void (*ow_cb_generic_t) (void);

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
