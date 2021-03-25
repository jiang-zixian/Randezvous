#include "board.h"
#include "pin_mux.h"
#include "fsl_common.h"

#include <stdio.h>

//=============================================================================
// RTC
//=============================================================================

void RTC_Init(void)
{
	RTC->COUNT = 0;
	RTC->CTRL |= RTC_CTRL_RTC_EN_MASK;
	RTC->CTRL |= RTC_CTRL_RTC_SUBSEC_ENA_MASK;
}

uint32_t RTC_GetTick(void)
{
	uint32_t seconds = RTC->COUNT;
	uint32_t subsecs = RTC->SUBSEC;

	return seconds * 1000 + subsecs * 1000 / 32768;
}

//=============================================================================
// Initialization
//=============================================================================

/*
 * Initialization routine called before .data and .bss are initialized.
 */
void SystemInitHook(void)
{
}

/*
 * Initialization routine called after .data and .bss are initialized.
 */
void __attribute__((constructor)) Init(void)
{
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitDebugConsole();

	RTC_Init();

	SDK_DelayAtLeastUs(2000000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
}

//=============================================================================
// Finalization
//=============================================================================

void __attribute__((destructor)) Fini(void)
{
	SDK_DelayAtLeastUs(2000000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);

	printf("\nBye!\n");
}