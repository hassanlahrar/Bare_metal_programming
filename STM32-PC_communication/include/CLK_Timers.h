#ifndef CLK_TIMERS_h
#define CLK_TIMERS_h
#include "stm32f103xb.h"

void systemClockConfig();
void TIMERS_Config(uint8_t TIM2_val, uint8_t TIM3_val, uint8_t TIM4_val);
void tim2_ON_court(void);
void tim2_OFF_court(void);
void tim3_ON_moyen(void);
void tim3_OFF_moyen(void);
void tim4_ON_long(void);
void tim4_OFF_long(void);



#endif