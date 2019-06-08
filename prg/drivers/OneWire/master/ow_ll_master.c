/**
* @file ow_ll_master.c
* @brief library 1-Wire(www.maxim-ic.com)
* @author Maciej Rajtar (Published 10 May 2010 www.mbed.org)
* @author Frederic BLANC
* @author Dorus van de Spijker 
*/
#include "ow_ll_master.h"
#include "ow_port_macro.h"
#include "crc8.h"

#include <string.h>

static ow_hal_master_t *master_hal;

uint8_t ow_ll_master_init(ow_hal_master_t *hal)
{
	if(hal == NULL) {
		return OW_INV_ARG;
	}

	master_hal = hal;

	return OW_OK;
}


uint8_t ow_ll_master_reset(void) { // reset.  Should improve to act as a presence pulse
    uint8_t err;

    master_hal->hal_io->output();
    master_hal->hal_io->write(0);     // bring low for 500 us
   OW_PORT_WAIT_US(OW_TR_MIN);
    master_hal->hal_io->input();
   OW_PORT_WAIT_US(OW_TP);
    err = master_hal->hal_io->read();
   OW_PORT_WAIT_US(OW_TP);
    if ( master_hal->hal_io->read() == 0 )    {    // short circuit
        err = OW_SHORT_CIRCUIT;
    }
    return err;
}

uint8_t ow_ll_master_transmit_receive_bit( uint8_t bit ) {

    master_hal->hal_io->output(); // drive bus low
    master_hal->hal_io->write(0);
   OW_PORT_WAIT_US(OW_TO_MIN); // Recovery-Time is 1
    
    if ( bit ) 
        master_hal->hal_io->input(); // if bit is 1 set bus high (by ext. pull-up)
    //  delay was 15uS-1 see comment above
    OW_PORT_WAIT_US(OW_TS);
    if ( master_hal->hal_io->read() == 0 ) bit = 0; // sample at end of read-timeslot
    OW_PORT_WAIT_US(OW_DTOZ_MIN);
    master_hal->hal_io->input();
	OW_PORT_WAIT_US(OW_TRC);
    return bit;
}

uint8_t ow_ll_master_transmit_byte( uint8_t byte ) {
    uint8_t i = 8, j;

    do {
        j = ow_ll_master_transmit_receive_bit( byte & 1 );
        byte >>= 1;
        if ( j )
            byte |= 0x80;
    } while ( --i );
    return byte;
}

uint8_t ow_ll_master_receive_byte( void ) {
    // read by sending 0xff (a dontcare?)
    return ow_ll_master_transmit_byte( 0xFF );
}

uint8_t ow_ll_master_parasite_enable(void) {
    master_hal->hal_io->output();
    master_hal->hal_io->write(1);
    return 0;
}

uint8_t ow_ll_master_parasite_disable(void) {

    master_hal->hal_io->input();
    return 0;
}
