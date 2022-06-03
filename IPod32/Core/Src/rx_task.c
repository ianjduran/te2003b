/*
 * rx_task.c
 *
 *  Created on: Jun 1, 2022
 *      Author: iandu
 */

#include "rx_tast.h"

extern osMessageQueueId_t rpiInQueueHandle;

uint8_t receive_data_task(void){
	uint8_t data;
	while (1){
		if((USART1->SR & USART_SR_RXNE)>0){
			data = USER_USART1_Receive();
			osMessageQueuePut(rpiInQueueHandle, &data, 0U, 50);
		}
		osDelay(1);
	}
}
