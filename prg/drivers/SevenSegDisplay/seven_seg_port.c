
#include "seven_seg_port.h"
#include "seven_seg_common.h"
#include "seven_seg_config.h"

#include "spi.h"
#include "tim.h"
#include "tim_config.h"
#include "gpio.h"
#include "stm32f1xx_hal.h"

#define SPI_WRITE_TIMEOUT_MS_DEFAULT 20
#define DISPLAY_TIMER_PRESCALER 64
#define DISPLAY_TIMER_SCALED_FREQ_HZ SEVEN_SEG_CONFIG_TIMER_FREQ_HZ / DISPLAY_TIMER_PRESCALER

static uint32_t SpiWriteTimeoutMs = SPI_WRITE_TIMEOUT_MS_DEFAULT;

int SevenSegPortInit(uint32_t interval_ms);
void SevenSegPortTimerStart(void);
void SevenSegPortDigitSelect(uint8_t digit_num);
void SevenSegPortDigitDeselect(uint8_t digit_num);
void SevenSegPortDigitSet(uint8_t digit_num, uint8_t segments);

static void SevenSegCallbackTimer(void);

void SevenSegPortBind(SevenSegHal_t *hal)
{
	if(hal != NULL) {
		hal->init = SevenSegPortInit;
		hal->timer_start = SevenSegPortTimerStart;
		hal->digit_select = SevenSegPortDigitSelect;
		hal->digit_deselect = SevenSegPortDigitDeselect;
		hal->digit_set = SevenSegPortDigitSet;
	}
}

int SevenSegPortInit(uint32_t update_freq_hz)
{
	uint32_t period = DISPLAY_TIMER_SCALED_FREQ_HZ / update_freq_hz;

	Spi7SdInit();
	Gpio7SdSelInit();
	TimerConfig_t tmr_sevenseg_cfg = {
		.timer = TIMER_DRIVER_SEVENSEG,
		.instance = TIM2,
		.irqn_elapsed = TIM2_IRQn,
		.cb_elapsed = NULL,
		.irq_prio_elapsed = 2,
		.irq_subprio_elapsed = 0,
	};
	TimerHwConfigSet(&tmr_sevenseg_cfg);
	TimerHwInit(TIMER_DRIVER_SEVENSEG, DISPLAY_TIMER_PRESCALER, period);
	TimerHwCallbackRegisterPeriodElapsed(TIMER_DRIVER_SEVENSEG, SevenSegCallbackTimer);

	SpiWriteTimeoutMs = (1000 / update_freq_hz) / 3;

	TimerHwIntEnablePeriodElapsed(TIMER_DRIVER_SEVENSEG, 1);

	return SEVEN_SEG_RES_OK;
}

void SevenSegPortTimerStart(void)
{
	TimerHwStart(TIMER_DRIVER_SEVENSEG);
}

void SevenSegPortDigitSelect(uint8_t digit_num)
{
	Gpio7SdSelStateSet(digit_num, 1);

}

void SevenSegPortDigitDeselect(uint8_t digit_num)
{
	Gpio7SdSelStateSet(digit_num, 0);
}

void SevenSegPortDigitSet(uint8_t digit_num, uint8_t segments)
{
	Gpio7SdSpiNssStateSet(0);
	HAL_SPI_Transmit(&hspi_7sd, &segments, sizeof(segments), SpiWriteTimeoutMs);
	Gpio7SdSpiNssStateSet(1);
}

static void SevenSegCallbackTimer(void)
{
	SevenSegHalCallbackDisplayUpdate();
}

