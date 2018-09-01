
#include "seven_seg_port.h"
#include "seven_seg_common.h"

#include "spi.h"
#include "tim.h"
#include "gpio.h"
#include "stm32f1xx_hal.h"

#define SPI_WRITE_TIMEOUT_MS_DEFAULT 20

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

int SevenSegPortInit(uint32_t interval_ms)
{
	uint32_t prescaler = 0;
	uint32_t period = 0;
	
	Spi7SdInit();
	Gpio7SdSelInit();
	TimerInit(TIMER_APP_0, prescaler, period);
	TimerCallbackRegisterPeriodElapsed(TIMER_APP_0, SevenSegCallbackTimer);
	SpiWriteTimeoutMs = interval_ms / 2;

	return SEVEN_SEG_RES_OK;
}

void SevenSegPortTimerStart(void)
{
	TimerStart(TIMER_APP_0);
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
	HAL_SPI_Transmit(hspi_7sd, &segments, sizeof(segments), SpiWriteTimeoutMs);
}

static void SevenSegCallbackTimer(void)
{
	SevenSegHalCallbackDisplayUpdate();
}

