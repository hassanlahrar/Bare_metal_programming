#include "str_to_blink.h"
#include "CLK_Timers.h"
#include "usart.h"
#include "decoding.h"



#define BUFFER_SIZE 255


uint8_t received_data[BUFFER_SIZE];
int data_ready = 0;

int main(void)
{
    systemClockConfig();
    init_UART2();

    while(1)
    {
        // Wait for data to be received
        while(!data_ready);

        // Reset the data ready flag
        data_ready = 0;

        decoding(received_data);
    }

    return 0;
}

void USART2_IRQHandler(void)    
{
    static int buffer_index = 0;

    // Check if a character was received
    if(USART2->SR & USART_SR_RXNE)
    {
        uint8_t data = USART2->DR;

        // Check if the character is a newline or the buffer is full
        if(data == '\n' || buffer_index >= BUFFER_SIZE-1)
        {
            received_data[buffer_index] = '\n';
            buffer_index = 0;
            data_ready = 1;
        }
        else
        {
            received_data[buffer_index] = data;
            buffer_index++;
        }
    }
}