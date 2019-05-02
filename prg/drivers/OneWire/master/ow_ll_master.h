/**
* @file ow_ll_master.h
* @brief library 1-Wire(www.maxim-ic.com)
* @author Maciej Rajtar (Published 10 May 2010 www.mbed.org)
* @author Frederic BLANC
* @author Dorus van de Spijker
*/

#ifndef ONEWIRE_LL_MASTER_H_
#define ONEWIRE_LL_MASTER_H_

#include "ow_hal_master.h"
#include "ow_common.h"

#include <stdint.h>

uint8_t ow_ll_master_init(ow_hal_master_t *hal);

/**
*    @brief onewire reset bus
*    @return pin ow or OW_SHORT_CIRCUIT
*    @date 20/06/2011
*/
uint8_t ow_ll_master_reset(void);

/**
*    @brief read write onewire
*    @param  [in/out] bit data
*    To read a bit always pass 1.
*    @return data
*    @date 20/06/2011
*/
uint8_t ow_ll_master_transmit_receive_bit( uint8_t bit );

/**
*    @brief byte write on onewire
*    @param  [in] byte data
*    @return data
*    @date 20/06/2011
*/
uint8_t ow_ll_master_transmit_byte( uint8_t byte );

/**
*    @brief byte read on onewire
*    @return uint8_t data byte
*    @date 20/06/2011
*/
uint8_t ow_ll_master_receive_byte( void );

/**
*     @brief parasite enable
*     @date 20/06/2011
*/
uint8_t ow_ll_master_parasite_enable(void);

/**
*     @brief parasite disable
*     @date 20/06/2011
*/
uint8_t ow_ll_master_parasite_disable(void);


#endif
