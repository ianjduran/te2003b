/*
 * rx_task.c
 *
 *  Created on: Jun 1, 2022
 *      Author: iandu
 */

#define max_line 29

#include "rx_tast.h"
#include "LCDTask/LCDTask.h"
#include "string.h"

extern osMessageQueueId_t rpiInQueueHandle;
uint8_t receive_data_task(void){
	uint32_t buf_len = 30;
	uint8_t buf[buf_len];
	uint32_t i = 0;
	uint32_t msg_len = 0;
	while (1){
		while(msg_len == 0 && i != buf_len){
			buf[i] = USER_USART1_Receive();
			if(buf[i]== '\0'){
				msg_len= i+1;
				i = 0;
				//end msg
				break;
			}
			i++;
		}

		if(msg_len>0){
			//Process msg
			set_lcd(buf, "Im hoping this works this time otherwise imma commit morision");

			msg_len = 0;

		}
		osDelay(1);
	}
}
