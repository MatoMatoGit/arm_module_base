/*
 * ow_cmd_master.c
 *
 *  Created on: May 1, 2019
 *      Author: dorus
 */

#include "ow_cmd_master.h"
#include "ow_ll_master.h"

uint8_t ow_cmd_master_init(ow_hal_master_t *hal)
{
	if(hal == NULL) {
		return OW_INV_ARG;
	}

	return ow_ll_master_init(hal);
}

uint8_t ow_cmd_master_transmit( uint8_t command, uint8_t id[] ) {
    uint8_t i;

    ow_ll_master_reset();
    if ( id ) {
        ow_ll_master_transmit_byte( OW_MATCH_ROM );            // to a single device
        i = OW_ROMCODE_SIZE;
        do {
            ow_ll_master_transmit_byte( *id );
            ++id;
        } while ( --i );
    } else {
        ow_ll_master_transmit_byte( OW_SKIP_ROM );            // to all devices
    }
    ow_ll_master_transmit_byte( command );
    return 0;
}

uint8_t ow_cmd_master_rom_search( uint8_t diff, uint8_t id[] ) {
    uint8_t i, j, next_diff;
    uint8_t b;

    if ( ow_ll_master_reset() )
        return OW_PRESENCE_ERR;    // error, no device found
    ow_ll_master_transmit_byte( OW_SEARCH_ROM );            // ROM search command
    next_diff = OW_LAST_DEVICE;            // unchanged on last device
    i = OW_ROMCODE_SIZE * 8;                    // 8 bytes
    do {
        j = 8;                                // 8 bits
        do {
            b = ow_ll_master_transmit_receive_bit( 1 );                // read bit
            if ( ow_ll_master_transmit_receive_bit( 1 ) ) {            // read complement bit
                if ( b )                    // 11
                    return OW_DATA_ERR;        // data error
            } else {
                if ( !b ) {                    // 00 = 2 devices
                    if ( diff > i || ((*id & 1) && diff != i) ) {
                        b = 1;                // now 1
                        next_diff = i;        // next pass 0
                    }
                }
            }
            ow_ll_master_transmit_receive_bit( b );                 // write bit
            *id >>= 1;
            if ( b )
                *id |= 0x80;            // store bit
            --i;
        } while ( --j );
        id++;                                // next byte
    } while ( i );
    return next_diff;                // to continue search
}

uint8_t ow_cmd_master_find_device(uint8_t family_code, uint8_t *diff, uint8_t id[]) {
    for (;;)
    {
        *diff = ow_cmd_master_rom_search( *diff, &id[0] );
        if ( *diff==OW_PRESENCE_ERR)
            return OW_ERROR;
        if ( *diff==OW_DATA_ERR )
            return OW_ERROR;
        if ( *diff == OW_LAST_DEVICE )
            return OW_OK ;
        if ( id[0] == family_code)
            return OW_OK ;
    }
}

uint8_t ow_cmd_master_search_devices(uint8_t family_code, uint8_t *n ,uint8_t *ids) {
    uint8_t i;
    uint8_t id[OW_ROMCODE_SIZE];
    uint8_t diff;
    printf( "Scanning Bus\n" );
    diff = OW_SEARCH_FIRST;
    for (*n = 0 ; (diff != OW_LAST_DEVICE) && (*n < MAXSENSORS) ;++(*n) ) {
    	ow_cmd_master_find_device( family_code, &diff, &id[0] );
        if ( diff == OW_PRESENCE_ERR ) {
            printf( "No Sensor found\n" );
            return diff;
        }
        if ( diff == OW_DATA_ERR ) {
            printf( "Bus Error\n" );
            return diff;
        }
		memcpy(&ids[(*n)*OW_ROMCODE_SIZE+i], id, OW_ROMCODE_SIZE);

    }
    return OW_OK;
}
