/**
* @file onewire.h
* @brief library 1-Wire(www.maxim-ic.com)
* @author Maciej Rajtar (Published 10 May 2010 www.mbed.org)
* @author Frederic BLANC
* @author Dorus van de SPijker
*/

#ifndef _onewire_
#define _onewire_

#include "onewire_pin.h"
#include <stdint.h>

#define MAX_DEVICES 16

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

typedef void (*ow_fn_wait_t) (uint32_t us);

typedef struct {
	ow_pin_t pin;
	ow_fn_wait_t wait_us;
} ow_config_t;

void ow_init(ow_config_t *config);

/**
*    @brief onewire reset bus
*    @return pin ow or OW_SHORT_CIRCUIT
*    @date 20/06/2011
*/
uint8_t ow_reset(void);

/**
*    @brief read write onewire
*    @param  [in/out] b data
*    @return data
*    @date 20/06/2011
*/
uint8_t ow_bit_io( uint8_t b );

/**
*    @brief byte write on onewire
*    @param  [in] b data
*    @return data
*    @date 20/06/2011
*/
uint8_t ow_byte_wr( uint8_t b );

/**
*    @brief byte read on onewire
*    @param  [in] uint8_t b
*    @return 
*    @date 20/06/2011
*/
uint8_t ow_byte_rd( void );

/**
*     @brief parasite enable
*     @date 20/06/2011
*/
uint8_t ow_parasite_enable(void);

/**
*     @brief parasite disable
*     @date 20/06/2011
*/
uint8_t ow_parasite_disable(void);

/**
*    @brief write command
*    @param  [in] command
*    @param  [in] id romcode
*    @date 20/06/2011
*/
uint8_t ow_command( uint8_t command, uint8_t id[] );

/**
*    @brief search romcode
*    @param  [in] uint8_t diff
*    @param  [out] id romcode
*    @return next_diff or OW_LAST_DEVICE or OW_DATA_ERR or OW_PRESENCE_ERR
*    @date 20/06/2011
*/
uint8_t ow_rom_search( uint8_t diff, uint8_t id[] );

/**
*    @brief find a device with specified family code on the 1-Wire-Bus
*    @param  [in] family_code familiy code to find
*    @param  [in/out] diff is the result of the last rom-search
*    @param  [out] is the rom-code of the sensor found
*    @return  OW_OK or OW_ERROR 
*/
uint8_t ow_find_device(uint8_t family_code, uint8_t *diff, uint8_t id[]);

/**
*    @brief search devices with specified family code on the 1-Wire-Bus
*    @param  [in] family_code family code to search for
*    @param  [out] n number of device onewire
*    @param  [out] ids[][] array of id romcodes
*    @return OW_OK or OW_PRESENCE_ERR or OW_DATA_ERR
*    @date 20/06/2011
*/
uint8_t ow_search_devices(uint8_t family_code, uint8_t *n ,uint8_t *ids);

/**
*    @brief print the ids
*    @param  [in] id[] = rom_code
*    @param  [in] n number of id[n]
*    @param  [out] text id
*    @return OW_OK or OW_ERROR_CRC
*    @date 20/06/2011
*/
uint8_t ow_show_id( uint8_t id[], size_t n ,char *text);

/**
*    @brief test pin onewire bus
*    @return pin state
*    @date 20/06/2011
*/
uint8_t ow_test_pin (void);

#endif
