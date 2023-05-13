#include <string.h>
#include "str_to_blink.h"
#include "CLK_Timers.h"
#include "decoding.h"

extern int data_ready;

void decoding(uint8_t *receivd_data){
    
    uint8_t loop_octet = receivd_data[0];
    uint8_t nb_iter_octet = receivd_data[1];
    uint8_t TIM2_octet = receivd_data[2];
    uint8_t TIM3_octet = receivd_data[3];
    uint8_t TIM4_octet = receivd_data[4];
    uint8_t nb_char_octet = receivd_data[5];
    uint8_t str[255];
    int j=0;
    while (j<nb_char_octet || receivd_data[j]=='\n')
    {
        str[j]= receivd_data[j+6];
        ++j;
    }

    TIMERS_Config(TIM2_octet,TIM3_octet,TIM4_octet);
    
    if(loop_octet==0){
        for(int i=0; i<nb_iter_octet; ++i){
            traitement(str, nb_char_octet);
        }
    }
    else if(loop_octet==1)
    {
         while(!data_ready){
        traitement(str, nb_char_octet);
        }
    }

}