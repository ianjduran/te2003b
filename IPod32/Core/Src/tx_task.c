/*
 * tx_task.c
 *
 *  Created on: Jun 2, 2022
 *      Author: iandu
 */

#include "tx_task.h"

void transmit_data_task(void){
	uint8_t play[] = "z\0";
	uint8_t next[] = "n\0";
	uint8_t prev[] = "p\0";
	uint8_t stop[] = "s\0";
	while(1){
		USER_USART1_Transmit(play, strlen(play));
		osDelay(1000);
		USER_USART1_Transmit(next, strlen(next));
		osDelay(1000);
		USER_USART1_Transmit(prev, strlen(prev));
		osDelay(1000);
		USER_USART1_Transmit(stop, strlen(stop));
		osDelay(1000);
		osDelay(1);
	}
}
