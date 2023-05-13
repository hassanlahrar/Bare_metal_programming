#include "stm32f103xb.h"
#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <string.h>


void systemClockConfig(void);
void init_UART1(void);
void init_Adc(void);
void init_Timer(void);
uint16_t adc_read(void);
void send_to_usart1_string(char* str);
void send_usart1(char c);



static uint16_t adc_value;
const float T0 = 25 + 273.15;
const float RT0 = 10000.0;
const float R0 = 10000.0;
const float B = 3950.0;
float T;
char buffer[50];

int main(void)
{
    
    systemClockConfig();
    init_UART1();
    init_Adc();
    init_Timer();

    while (1)
    {
        __asm("nop");
    }

    return 0;
}


void systemClockConfig(){

    // Conf clock : 72MHz using HSE 8MHz crystal w/ PLL X 9 (8MHz x 9 = 72MHz)
    FLASH->ACR      |= FLASH_ACR_LATENCY_2; // Two wait states, per datasheet
    RCC->CFGR       |= RCC_CFGR_PPRE1_2;    // prescale AHB1 = HCLK/2
    RCC->CR         |= RCC_CR_HSEON;        // enable HSE clock
    while( !(RCC->CR & RCC_CR_HSERDY) );    // wait for the HSEREADY flag
    
    RCC->CFGR       |= RCC_CFGR_PLLSRC;     // set PLL source to HSE
    RCC->CFGR       |= RCC_CFGR_PLLMULL9;   // multiply by 9
    RCC->CR         |= RCC_CR_PLLON;        // enable the PLL
    while( !(RCC->CR & RCC_CR_PLLRDY) );    // wait for the PLLRDY flag
    
    RCC->CFGR       |= RCC_CFGR_SW_PLL;     // set clock source to pll

    while( !(RCC->CFGR & RCC_CFGR_SWS_PLL) );    // wait for PLL to be CLK
    
    SystemCoreClockUpdate();


     /* RCC */
	RCC->APB2ENR |= (1 << 2); // Enable RCC for GPIOA Pin/Port
	
	/* MODER */
	GPIOA->CRL &= ~((1 << 22) | (1 << 23) | (1 << 21) | (1 << 20)); // Clear GPIO PA5 MODE

	/* OUTPUT */
	GPIOA->CRL |= (1 << 21); // Set GPIO PA5 MODE (01 : output)
}

    void init_UART1() {
    
/* Enable USART1 clock */

    RCC->APB2ENR    |= RCC_APB2ENR_IOPAEN;      // enable GPIOA clock
    RCC->APB2ENR    |= RCC_APB2ENR_USART1EN;    // enable USART1 clock
    
    GPIOA->CRH &= ~(GPIO_CRH_CNF9  | GPIO_CRH_MODE9);   // reset PA9
    GPIOA->CRH &= ~(GPIO_CRH_CNF10 | GPIO_CRH_MODE10);  // reset PA10
    
    GPIOA->CRH |= GPIO_CRH_MODE9_1 | GPIO_CRH_MODE9_0;  // 0b11 50MHz output
    GPIOA->CRH |= GPIO_CRH_CNF9_1;    // PA9: output @ 50MHz - Alt-function Push-pull
    GPIOA->CRH |= GPIO_CRH_CNF10_0;   // PA10 RX - Mode = 0b00 (input) - CNF = 0b01 (input floating)

    uint32_t baud   = (uint32_t)(SystemCoreClock/115200);
    USART1->BRR     = baud;
    USART1->CR1     = USART_CR1_TE | USART_CR1_RE  | USART_CR1_UE;

    /* 8 data bits, parity control disabled, parity even */
    USART1->CR1 &= ~(USART_CR1_M | USART_CR1_PCE | USART_CR1_PS);

    /* 1 stop bit */
    USART1->CR2 &= ~(USART_CR2_STOP_Msk);

}

void init_Adc(){
    
    /*ADC clock must be < 14MHz; Config prescale 72MHz / 6 = 12Mhz*/

    RCC->CFGR |= RCC_CFGR_ADCPRE_DIV6;
    /*init GPIOA 2e bit*/
	
	RCC -> APB2ENR |= (1 << 2);
	
    /* init ADC1 9e bit*/
	
	RCC -> APB2ENR |= (1 << 9);
	
    /* init PA0 mode */
	GPIOA->CRL &= ~(0b1111 << 0);

    /*ALLUMER ADC1 */
	ADC1->CR2 |= (ADC_CR2_CONT | ADC_CR2_ADON);
	for( unsigned int i = 0 ; i < 1000 ; i++){
		__ASM("nop");
	}
	
    /* calibration du ADC (3e bit) CAL*/
	ADC1->CR2 |= ( 1 << 2);
	
	while(ADC1->CR2 & ( 1 << 2) ){}
		
    /* configurer la sequence */
		//SQR1 -> longeur
		ADC1->SQR1 &= ~(0xF << 20);
    /*sQR3 -> quel canal */
		ADC1->SQR3 &= ~(0b0000 <<0);
		
    /* sampling time channel 0 -> 0 to 28.5 cycles */
        ADC1->SMPR2 = ADC_SMPR2_SMP0_1;
}

void init_Timer(void){
    /*Activer le clock pour TIM2*/
    RCC->APB1ENR &= 0 << 0;
     RCC->APB1ENR |= 1 << 0;

    /*Choisir le prescaler*/
    TIM2->PSC = 59999; /*diviser la frequence source par 36000*/

    /*Choisir la valeur de compteur*/
    TIM2->ARR = 1199; /*interruption tous les 1000ms*/

    /*mise a jour des valeurs psc, arr */
    TIM2->EGR |= 1;

    /*Activer les interruptions*/
    TIM2->DIER |= 1;

    NVIC_EnableIRQ(TIM2_IRQn);

    /*start le timer*/
    TIM2->CR1 |= 1;
}

void TIM2_IRQHandler(void){
   if ( TIM2->SR  & 1)
   {
    TIM2->SR &= ~1;
    uint16_t raw = adc_read();
    T=1/((1/T0) + (1/B)*log((R0/RT0)*((4096.0/raw)-1))) - 273.15;
    sprintf(buffer, "tempe: %2.1f\r\n", T0);
    send_to_usart1_string(buffer);
    GPIOA->ODR ^= (1 << 5); //blink led to check that data is transmitted
    
	}   
    
}

uint16_t adc_read(){

    /* start conversion swstart */
        ADC1->CR2 &= ~(1 << 22);
		ADC1->CR2 |= (1 << 22);
		
/* wait end of conversion EOC */
		while(!(ADC1->SR & (1 << 1))){}
		
/* lire la donnee */
		 adc_value = ADC1->DR;
         if(adc_value){
            __asm("nop");
         }
         return adc_value;
}

void send_to_usart1_string(char *str)
{
         while (*str)
         {
            send_usart1(*str++);
         }
}

void send_usart1(char c)
{
         while (!(USART1->SR & USART_SR_TXE));
         USART1->DR = c;
}