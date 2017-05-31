#include "LPC11uxx.h"	// LPC11uxx Peripheral Registers
#include "type.h"

volatile static uint32_t TimeTick = 0;

//--- SysTick Event Handler ---
void SysTick_Handler(void)	// SysTick interrupt happens every 10 msec
{
  TimeTick++;
}
//---


//--- SysTick Functions ---
uint32_t Systick_Gettime()
{
	return TimeTick;
}


void Systick_Start()
{
	TimeTick=0;
	SysTick->VAL = 0;	// Clear SysTick Counter
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;	// Enable SysTick Counter
}


void Systick_Stop()
{
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;	// Disable SysTick Counter
	SysTick->VAL = 0;	// Clear SysTick Counter
}


void delaySysTick(uint32_t tick)
{
	uint32_t timetick;

// Initialize
	SysTick->VAL = 0;	// Clear SysTick Counter
// Count
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;	// Enable SysTick Counter
	timetick = TimeTick;
	while ((TimeTick - timetick) < tick);
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;	// Disable SysTick Counter
// Finalize
  	SysTick->VAL = 0;	// Clear SysTick Counter
//---
  	return;
}


void wait_ms(uint32_t time)
{
	delaySysTick(time);
}


void wait_sec(uint32_t sec)
{
	while(sec--)
	{
		wait_ms(1000);
	}
}
//---
