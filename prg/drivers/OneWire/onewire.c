/**
* @file onewire.c
* @brief library 1-Wire(www.maxim-ic.com)
* @author Maciej Rajtar (Published 10 May 2010 www.mbed.org)
* @author Frederic BLANC
* @author Dorus van de Spijker 
*/
#include "onewire.h"
#include "crc8.h"

#include <string.h>

static ow_io_t *ow_io;
static ow_fn_wait_t *wait_us;

void ow_init(ow_config_t *config)
{
	ow_io = config->io;
	wait_us = config->wait_us;
}


uint8_t ow_reset(void) { // reset.  Should improve to act as a presence pulse
    uint8_t err;

    ow_io->output();
    ow_io->write(0);     // bring low for 500 us
    wait_us(500);
    ow_io->input();
    wait_us(60);
    err = ow_io->read();
    wait_us(240);
    if ( ow_io->read() == 0 )    {    // short circuit
        err = OW_SHORT_CIRCUIT;
    }
    return err;
}

uint8_t ow_bit_io( uint8_t bit ) {

    ow_io->output(); // drive bus low
    ow_io->write(0);
    wait_us(1); // Recovery-Time is 1
    
    if ( bit ) 
        ow_io->input(); // if bit is 1 set bus high (by ext. pull-up)
    //  delay was 15uS-1 see comment above
    wait_us(15-1);
    if ( ow_io->read() == 0 ) bit = 0; // sample at end of read-timeslot
    wait_us(60-15);
    ow_io->input();
    return bit;
}

uint8_t ow_byte_wr( uint8_t byte ) {
    uint8_t i = 8, j;

    do {
        j = ow_bit_io( byte & 1 );
        byte >>= 1;
        if ( j )
            byte |= 0x80;
    } while ( --i );
    return byte;
}

uint8_t ow_byte_rd( void ) {
    // read by sending 0xff (a dontcare?)
    return ow_byte_wr( 0xFF );
}

uint8_t ow_parasite_enable(void) {
    ow_io->output();
    ow_io->write(1);
    return 0;
}

uint8_t ow_parasite_disable(void) {

    ow_io->input();
    return 0;
}

uint8_t ow_command( uint8_t command, uint8_t id[] ) {
    uint8_t i;

    ow_reset();
    if ( id ) {
        ow_byte_wr( OW_MATCH_ROM );            // to a single device
        i = OW_ROMCODE_SIZE;
        do {
            ow_byte_wr( *id );
            ++id;
        } while ( --i );
    } else {
        ow_byte_wr( OW_SKIP_ROM );            // to all devices
    }
    ow_byte_wr( command );
    return 0;
}

uint8_t ow_rom_search( uint8_t diff, uint8_t id[] ) {
    uint8_t i, j, next_diff;
    uint8_t b;

    if ( ow_reset() )
        return OW_PRESENCE_ERR;    // error, no device found
    ow_byte_wr( OW_SEARCH_ROM );            // ROM search command
    next_diff = OW_LAST_DEVICE;            // unchanged on last device
    i = OW_ROMCODE_SIZE * 8;                    // 8 bytes
    do {
        j = 8;                                // 8 bits
        do {
            b = ow_bit_io( 1 );                // read bit
            if ( ow_bit_io( 1 ) ) {            // read complement bit
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
            ow_bit_io( b );                 // write bit
            *id >>= 1;
            if ( b ) 
                *id |= 0x80;            // store bit
            --i;
        } while ( --j );    
        id++;                                // next byte
    } while ( i );
    return next_diff;                // to continue search
}

uint8_t ow_find_device(uint8_t family_code, uint8_t *diff, uint8_t id[]) {
    for (;;) 
    {
        *diff = ow_rom_search( *diff, &id[0] );
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

uint8_t ow_search_devices(uint8_t family_code, uint8_t *n ,uint8_t *ids) {
    uint8_t i;
    uint8_t id[OW_ROMCODE_SIZE];
    uint8_t diff;
    printf( "Scanning Bus\n" );
    diff = OW_SEARCH_FIRST;
    for (*n = 0 ; (diff != OW_LAST_DEVICE) && (*n < MAXSENSORS) ;++(*n) ) {
        ow_find_device( family_code, &diff, &id[0] );
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

uint8_t ow_show_id( uint8_t id[], size_t n ,char *text) {
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

uint8_t ow_test_pin (void){
    if (ow_io->read())
        return 1;
    return 0;
}

/**
*     @brief output byte d (least sig bit first)
*    @param  [in] d output byte (least sig bit first)
*    @return  OW_OK 
*/
uint8_t OneWireOutByte(uint8_t d) { // output byte d (least sig bit first).
    for (int n=8; n!=0; n--) {
        if ((d & 0x01) == 1) { // test least sig bit
            ow_io->output();
            ow_io->write(0);
            wait_us(5);
            ow_io->input();
            wait_us(80);
        } else {
            ow_io->output();
            ow_io->write(0);
            wait_us(80);
            ow_io->input();
        }
        d=d>>1; // now the next bit is in the least sig bit position.
    }
    return OW_OK;
}
/**
*     @brief read byte, least sig byte first
*    @return  byte, least sig byte first 
*/
uint8_t OneWireInByte(void) { // read byte, least sig byte first
    uint8_t d = 0, b;
    for (int n=0; n<8; n++) {
        ow_io->output();
        ow_io->write(0);
        wait_us(5);
        ow_io->input();
        wait_us(5);
        b = ow_io->read();
        wait_us(50);
        d = (d >> 1) | (b << 7); // shift d to right and insert b in most sig bit position
    }
    return d;
}


