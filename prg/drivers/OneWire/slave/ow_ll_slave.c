
#include "ow_ll_slave.h"
#include "ow_hal_slave.h"

typedef enum {
	OW_SLAVE_STATE_WAIT_FALLING = 0,
	OW_SLAVE_STATE_WAIT_RISING,
} ow_slave_state_t;

#define DELTA_RESET_MIN		480
#define DELTA_RESET_MAX		520
#define DELTA_RX_ONE_MAX	15
#define DELTA_RX_ZERO_MIN	20
#define DELTA_RX_ZERO_MAX	65
#define DELAY_TX_ZERO		30
#define DELTA_TX_PRESENCE	60

static void ow_ll_slave_write_bit(uint8_t bit);

ow_hal_slave_t *slave_hal = NULL;
ow_ll_slave_cbs_t *slave_cbs = NULL;

uint8_t ow_ll_slave_init(ow_hal_slave_t *hal, ow_ll_slave_cbs_t *cbs)
{
	if(hal == NULL || cbs == NULL) {
		return OW_INV_ARG;
	}

	slave_hal = hal;
	slave_cbs = cbs;

	slave_hal->hal_io->input();
	slave_hal->hal_int->int_init();
	slave_hal->hal_tmr->tmr_init();
	slave_hal->hal_int->int_enable();

	return OW_OK;
}

void ow_ll_slave_start(void)
{
	slave_hal->hal_int->int_enable();
}

void ow_ll_slave_stop(void)
{
	slave_hal->hal_int->int_disable();
}

void ow_ll_slave_int_handler(void)
{
	static ow_slave_state_t state = OW_SLAVE_STATE_WAIT_FALLING;
	static uint32_t tp = 0;
	uint32_t dt;
	uint32_t tn;

	/* Get the current time in usec,
	 * calculate the delta between now and
	 * the time of the previous interrupt. */
	tn = slave_hal->hal_tmr->tmr_get_us();
	dt = tn - tp;
	tp = tn; /* Previous time becomes current time. */
	
	uint8_t tx_bit = 0;
	
	switch(state) {
		case OW_SLAVE_STATE_WAIT_FALLING: {
			slave_hal->hal_int->int_set_rising();
			state = OW_SLAVE_STATE_WAIT_RISING;
			break;
		}
		
		case OW_SLAVE_STATE_WAIT_RISING: {
			slave_hal->hal_int->int_disable();
			
			if(dt <= DELTA_RX_ONE_MAX) {
				tx_bit = slave_cbs->receive_transmit_bit(1);
				ow_ll_slave_write_bit(tx_bit);
			} else if(dt >= DELTA_RX_ZERO_MIN && dt <= DELTA_RX_ZERO_MAX) {
				slave_cbs->receive_transmit_bit(0);
			} else if(dt >= DELTA_RESET_MIN && dt <= DELTA_RESET_MAX) {
				ow_ll_slave_write_bit(0); /* Presence pulse ~60us. */
				slave_cbs->reset();
			} else {
				slave_cbs->error();
			}
			
			slave_hal->hal_int->int_set_falling();
			slave_hal->hal_int->int_enable();
			state = OW_SLAVE_STATE_WAIT_FALLING;
			break;
		}
	}
}

static void ow_ll_slave_write_bit(uint8_t bit)
{
	/* Only pull the line low if the slave
	 * wants to transmit a 0. */
	if(bit == 0) {
		slave_hal->hal_io->output();
		slave_hal->hal_io->write(0);
		slave_hal->hal_gen->wait_us(DELAY_TX_ZERO);
		slave_hal->hal_io->input();
	}
}
