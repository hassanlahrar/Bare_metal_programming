#include "stm32f103xb.h"
#include "FreeRTOS.h"
#include "task.h"
#include <unistd.h>

#define LCD_RS_ON() GPIOA->ODR |= 1<<0;
#define LCD_RS_OFF() GPIOA->ODR &= ~(1<<0);

#define LCD_E_ON() GPIOA->ODR |= 1<<1;
#define LCD_E_OFF() GPIOA->ODR &= ~(1<<1);

#define LCD_PA2_ON() GPIOA->ODR |= 1<<2;
#define LCD_PA2_OFF() GPIOA->ODR &= ~(1<<2);

#define LCD_PA3_ON() GPIOA->ODR |= 1<<3;
#define LCD_PA3_OFF() GPIOA->ODR &= ~(1<<3);

#define LCD_PA4_ON() GPIOA->ODR |= 1<<4;
#define LCD_PA4_OFF() GPIOA->ODR &= ~(1<<4);

#define LCD_PA5_ON() GPIOA->ODR |= 1<<6;
#define LCD_PA5_OFF() GPIOA->ODR &= ~(1<<6);



void SystemClockConfig(void);
void init_Timer(void);
void init_GPIO(void);
void init_RTC(void);
void _lcdWrite4bits(uint8_t data, uint8_t rs);
void _lcdWrite8bits(uint8_t data, uint8_t rs);
void lcdInit();
void lcdPutc(char c);
static void delay_us(int us);

void vTask1(void *pvParameters);
void vTask2(void *pvParameters);


TaskHandle_t clock_hour = NULL, display = NULL;


int main(void){
	SystemClockConfig();
	init_Timer();
	init_GPIO();
	init_RTC();
	lcdInit();

	 xTaskCreate(&vTask1, "Task 1", 128, NULL, 1, &clock_hour);
	 xTaskCreate(&vTask2, "Task 2", 128, NULL, 1, &display);


	vTaskStartScheduler();

	while(1);

return 0;

}

void vTask1(void *pvParameters)
{
    while(1)
    {
		while((RTC->CRL& RTC_CRL_RSF) != RTC_CRL_RSF);

		uint32_t hour = RTC->CNTH & 0x000F;
		hour = (hour << 4) | ((RTC->CNTH >> 4) & 0x000F);
		hour = (hour << 1) | ((RTC->CNTH >> 14) & 0x0001);
		uint32_t minute = RTC->CNTL & 0x00FF;
		minute = (minute << 4) | ((RTC->CNTL >> 4) & 0x000F);
		uint32_t second = RTC->CRL & 0x00FF;
		second = (second << 4) | ((RTC->CRL >> 4) & 0x000F);


    }
}

void vTask2(void *pvParameters)
{
    while(1)
    {
		lcdPutc('A');
    }
}

void _lcdWrite4bits(uint8_t data, uint8_t rs){
	if(rs){
		LCD_RS_ON();
	}
	else{
		LCD_RS_OFF();
	}

	LCD_E_ON();

	if(data & 1){
		LCD_PA2_ON();
	}
	else{
		LCD_PA2_OFF();
	}

	if(data & 2){
		LCD_PA3_ON();
	}
	else{
		LCD_PA3_OFF();
	}

	if(data & 4){
		LCD_PA4_ON();
	}
	else{
		LCD_PA4_OFF();
	}

	if(data & 8){
		LCD_PA5_ON();
	}
	else{
		LCD_PA5_OFF();
	}

	delay_us(1);

	LCD_E_OFF();

	delay_us(1);

}
void _lcdWrite8bits(uint8_t data, uint8_t rs){
	_lcdWrite4bits(data>>4,rs);
	_lcdWrite4bits(data,rs);
	delay_us(40);
}
void lcdInit(){
	delay_us(20000);
	_lcdWrite4bits(3,0);
	delay_us(5000);
	_lcdWrite4bits(3,0);
	delay_us(2000);
	_lcdWrite4bits(3,0);
	delay_us(2000);
	_lcdWrite4bits(2,0); //mode 4 bits actived
	delay_us(2000);
	_lcdWrite8bits(0x28,0); //mode 4 bits - 2 lines
	_lcdWrite8bits(0x08,0); //LCD off
	_lcdWrite8bits(0x01,0); // Clear LCD
	delay_us(3000);
	_lcdWrite8bits(0x06,0); //Incremente to the left
	_lcdWrite8bits(0x0F,0); // LCD ON, cursor blinks
}
void lcdPutc(char c){
	_lcdWrite8bits(c,1);
}

void SystemClockConfig(){

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
	GPIOA->CRL &= ~((1 << 22) | (1 << 23) | (1 << 21) | (1 << 20)); // Clear GPIO PA5 MODE
	GPIOA->CRL |= (1 << 21); // Set GPIO PA5 MODE (01 : output)
}

void init_GPIO(void){
	RCC->APB2ENR &=~(1 << 2); // clear RCC for GPIOA
	RCC->APB2ENR |= (1 << 2); // Enable RCC for GPIOA

	GPIOA->CRL &= ~((1 << 3) | (1 << 2) | (1 << 1) | (1 << 0)); // Clear GPIO PA0 MODE
	GPIOA->CRL &= ~((1 << 6) | (1 << 5) | (1 << 7) | (1 << 4)); // Clear GPIO PA1 MODE
	GPIOA->CRL &= ~((1 << 11) | (1 << 10) | (1 << 9) | (1 << 8)); // Clear GPIO PA2 MODE
	GPIOA->CRL &= ~((1 << 15) | (1 << 14) | (1 << 13) | (1 << 12)); // Clear GPIO PA3 MODE
	GPIOA->CRL &= ~((1 << 16) | (1 << 17) | (1 << 18) | (1 << 19)); // Clear GPIO PA4 MODE
	GPIOA->CRL &= ~((1 << 27) | (1 << 26) | (1 << 25) | (1 << 24)); // Clear GPIO PA5 MODE

	GPIOA->CRL |= 0x02222222; // Set GPIO PA0--->5 MODE (01 : output)
}

void init_RTC(void){
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	PWR->CR = PWR_CR_DBP;
	RCC->BDCR |= RCC_BDCR_LSEON;
	while (!(RCC->BDCR & RCC_BDCR_LSERDY));
	RCC->BDCR |= RCC_BDCR_RTCSEL_LSE;
	RCC->BDCR |= RCC_BDCR_RTCEN;

	RTC->CRL |= RTC_CRL_CNF;
	RTC->PRLL = 32767;
	RTC->CNTH = 0;
	RTC->CNTL = 0;
	RTC->CRL &=~RTC_CRL_CNF;
	RTC->CRH |= RTC_CRH_SECIE;
}

void init_Timer(void){
	    RCC->APB1ENR &= 0 << 0;
	    RCC->APB1ENR |= 1 << 0;
	    TIM2->PSC = 71; 
	    TIM2->ARR = 30000; 
	    TIM2->EGR |= 1;
	    TIM2->CR1 |= 1;
}

static void delay_us(int us)
{
        TIM2->CNT = 0;
	while(TIM2->CNT <= us );	
}