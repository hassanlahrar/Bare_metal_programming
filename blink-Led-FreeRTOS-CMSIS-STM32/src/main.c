#include "stm32f103xb.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include <unistd.h>

void SystemClockConfig(void);
void vTask1(void *pvParameters);

TaskHandle_t ma_tache_1 = NULL;
TimerHandle_t xTimer = NULL;

void vTimerCallback(TimerHandle_t xTimer){
	GPIOA->ODR ^= (1 << 5);
     
}

int main(void){
	SystemClockConfig();

	 xTaskCreate(&vTask1, "Task 1", 128, NULL, 1, &ma_tache_1);

	xTimer = xTimerCreate("Timer", pdMS_TO_TICKS(100),pdTRUE, 0, vTimerCallback);
    
	xTimerStart(xTimer, 0);
	vTaskStartScheduler();

	while(1);

return 0;

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
	GPIOC->CRH &= ~((1 << 22) | (1 << 23) | (1 << 21) | (1 << 20)); // Clear GPIO PC13 MODE
	
	/* OUTPUT */
	GPIOA->CRL |= (1 << 21); // Set GPIO PA5 MODE (01 : output)
	
	/* INPUT */
	GPIOC->CRH |= (1 << 23); // Set GPIO PC13 MODE (10 : input)
}


void vTask1(void *pvParameters)
{
    while(1)
    {
		int a = 1;
		a++;
    }
}
