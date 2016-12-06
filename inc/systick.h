extern void SysTick_Handler(void);
extern uint32_t Systick_Gettime();
extern void Systick_Start();
extern void Systick_Stop();
extern void delaySysTick(uint32_t tick);
extern void wait_ms(unsigned long time);
extern void wait_sec(unsigned long sec);
