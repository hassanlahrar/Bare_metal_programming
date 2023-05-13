#include <stdint.h>
#include <system_stm32f1xx.h>
#include <stm32f1xx.h>
#include "FreeRTOS.h"
#include "task.h"

void SystemClockConfig(void);
void vTask1(void *pvParameters);
void vTask2(void *pvParameters);
static void init_delay_timer(void);
static void delay_ms(void);


TaskHandle_t ma_tache_1 = NULL, ma_tache_2 = NULL;



int main(void){
	SystemClockConfig();
	init_delay_timer();


	 xTaskCreate(vTask1, "Task 1", configMINIMAL_STACK_SIZE, NULL, 1, &ma_tache_1);
	 xTaskCreate(vTask2, "Task 2", configMINIMAL_STACK_SIZE, NULL, 1, &ma_tache_2);

    
	vTaskStartScheduler();

	while(1);

}

void SystemClockConfig(void){
	//Activation HSI (HSION)
	RCC->CR &= 0;
	RCC->CR |= 1;
	//Attendre l'activation de HSI (HSIRDY)
	while( !(RCC->CR & (1<<1)) ){};
		
	//PLL prendre un facteur 9
	RCC->CFGR |= (0b0111 << 18);
	//Activer le PLL -> PLLON
	RCC->CR |= (1 << 24);
	//Attendre l'activation de PLL ->PLLRDY
	while( !(RCC->CR & (1<<25)) ){};
		
	//Mettre PLL comme clock source /SW
	RCC->CFGR &= ~0b11;
	RCC->CFGR |= 0b10;
	//Attendre le changement de clock source /SWS
	while(!(RCC->CFGR & (0b10<<2))){};
		
	//Prescaler 2 pour APB1
	RCC->CFGR &= ~(0b111 << 8);
	RCC->CFGR |= (0b100 << 8);
	
	/* RCC */
	RCC->APB2ENR |= (1 << 4) | (1 << 2); // Enable RCC for GPIOC, GPIOA Pin/Port
	
	/* MODER */
	GPIOA->CRL &= ~((1 << 22) | (1 << 23) | (1 << 21) | (1 << 20)); // Clear GPIO PA5 MODE
	
	/* OUTPUT */
	GPIOA->CRL |= (1 << 21); // Set GPIO PA5 MODE (01 : output)
	
}


void vTask1(void *pvParameters)
{
    while(1){
    
          GPIOA->ODR ^= (1 << 5);
          delay_ms();
    }
}

void vTask2(void *pvParameters)
{
    while(1)
    {
	 GPIOA->ODR &= ~(1 << 5);
          delay_ms();
	  
         
    }
}

static void init_delay_timer(void)
{
	    /*Activer le clock pour TIM2*/
	    RCC->APB1ENR &= 0 << 0;
	     RCC->APB1ENR |= 1 << 0;

	    /*Choisir le prescaler*/
	    TIM2->PSC = 35999; /*diviser la frequence source par 36000*/

	    /*Choisir la valeur de compteur*/
	    TIM2->ARR = 1999; /*interruption tous les 1000ms*/

	    /*mise a jour des valeurs psc, arr */
	    TIM2->EGR |= 1;
	    
	    /*start le timer*/
	    TIM2->CR1 |= 1;
}

static void delay_ms(void)
{
        TIM2->CNT = 0;
	while(TIM2->CNT < 999 );
	
	
}






