/*
 * ow_common.c
 *
 *  Created on: May 1, 2019
 *      Author: dorus
 */

#include "ow_common.h"
#include "crc8.h"

#include <stdio.h>
#include <string.h>

uint8_t ow_common_show_id( uint8_t id[], size_t n ,char *text) {
    size_t i;
    char hex[4];
    sprintf(text,"");

    for ( i = 0; i < n; i++ ) {
        if ( i == 0 ) strcat(text, " FC: " );
        else if ( i == n-1 ) strcat(text, "CRC: " );
        if ( i == 1 ) strcat(text, " SN: " );
        sprintf(hex,"%2.2X ",id[i]);
        strcat(text,hex);
    }
    if ( crc8( id, OW_ROMCODE_SIZE) )
        return OW_ERROR_CRC;
    return OW_OK;
}
