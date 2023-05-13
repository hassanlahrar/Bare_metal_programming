#include "CLK_Timers.h"


void systemClockConfig(){

    RCC->CR |= RCC_CR_HSION;

    /* Wait until HSI is ready */
    while (!(RCC->CR & RCC_CR_HSIRDY)) {
        ;
    }

    RCC->CFGR &= ~RCC_CFGR_PLLMULL16;
    RCC->CFGR |= RCC_CFGR_PLLMULL9;

    /* RCC->CFGR &= ~(RCC_CFGR_PPRE1);
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2; */

    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY)) {
        ;
    }

    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {
        ;
    }

    /*Activer le clock pour TIM2,3,4*/
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN | RCC_APB1ENR_TIM3EN | RCC_APB1ENR_TIM4EN;

     /* RCC */
	RCC->APB2ENR |= (1 << 2); // Enable RCC for GPIOA Pin/Port
	
	/* MODER */
	GPIOA->CRL &= ~((1 << 22) | (1 << 23) | (1 << 21) | (1 << 20)); // Clear GPIO PA5 MODE

	/* OUTPUT */
	GPIOA->CRL |= (1 << 21); // Set GPIO PA5 MODE (01 : output)

}

void TIMERS_Config(uint8_t TIM2_val, uint8_t TIM3_val, uint8_t TIM4_val){
    

    /*Choisir le prescaler pour TIM2,3,4*/
    TIM2->PSC = 35999; /*diviser la frequence source par 36000*/
    TIM3->PSC = 35999;
    TIM4->PSC = 35999;

    /*Choisir la valeur de compteur pour TIM2,3,4*/
    TIM2->ARR = (TIM2_val*10)-1; /*interruption tous les 500ms*/
    TIM3->ARR = (TIM3_val*10)-1; /*interruption tous les 1000ms*/
    TIM4->ARR = (TIM4_val*10)-1; /*interruption tous les 2000ms*/

    TIM2->CR1 |= TIM_CR1_ARPE;
    TIM3->CR1 |= TIM_CR1_ARPE;
    TIM4->CR1 |= TIM_CR1_ARPE;
    TIM2->SR &= ~TIM_SR_UIF;
    TIM3->SR &= ~TIM_SR_UIF;
    TIM4->SR &= ~TIM_SR_UIF;
    
    /*mise a jour des valeurs psc, arr */
    //TIM2->EGR |= 1;
}

void tim2_ON_court(void)
{
	TIM2->CR1 |= TIM_CR1_CEN; //activer timer2
    GPIOA->ODR |= (1 << 5);   //allumer LED
    while(!(TIM2->SR & TIM_SR_UIF)){  //tant y'a pas de débordement reste allumer
        __ASM("nop");
    }
    TIM2->SR &= ~TIM_SR_UIF;   //mettre le flag à 0
    GPIOA->ODR &= ~(1 << 5);   //etteind LED
    TIM2->CR1 &= ~TIM_CR1_CEN; //désactiver le timer2
}
void tim2_OFF_court(void)
{
	TIM2->CR1 |= TIM_CR1_CEN;  //activer le timer2
    GPIOA->ODR &= ~(1 << 5);   //etteind led
    while(!(TIM2->SR & TIM_SR_UIF)){ //tant y'a pas de débordement reste etteind
        __ASM("nop");
    }
    TIM2->SR &= ~TIM_SR_UIF;  //mettre le flag à 0
    TIM2->CR1 &= ~TIM_CR1_CEN;  //désactiver le timer2
}
void tim3_ON_moyen(void)
{
	TIM3->CR1 |= TIM_CR1_CEN;
    GPIOA->ODR |= (1 << 5);
    while(!(TIM3->SR & TIM_SR_UIF)){
        __ASM("nop");
    }
    TIM3->SR &= ~TIM_SR_UIF;
     GPIOA->ODR &= ~(1 << 5);
    TIM3->CR1 &= ~TIM_CR1_CEN;
}
void tim3_OFF_moyen(void)
{
	TIM3->CR1 |= TIM_CR1_CEN;
     GPIOA->ODR &= ~(1 << 5);
    while(!(TIM3->SR & TIM_SR_UIF)){
        __ASM("nop");
    }
    TIM3->SR &= ~TIM_SR_UIF;
    TIM3->CR1 &= ~TIM_CR1_CEN;
}
void tim4_ON_long(void)
{
	TIM4->CR1 |= TIM_CR1_CEN;
    GPIOA->ODR |= (1 << 5);
    while(!(TIM4->SR & TIM_SR_UIF)){
        __ASM("nop");
        }
    TIM4->SR &= ~TIM_SR_UIF;
     GPIOA->ODR &= ~(1 << 5);
    TIM4->CR1 &= ~TIM_CR1_CEN;
}
void tim4_OFF_long(void)
{
	TIM4->CR1 |= TIM_CR1_CEN;
     GPIOA->ODR &= ~(1 << 5);
    while(!(TIM4->SR & TIM_SR_UIF)){
        __ASM("nop");
    }
    TIM4->SR &= ~TIM_SR_UIF;
    TIM4->CR1 &= ~TIM_CR1_CEN;
}
