/*
 * rx_task.c
 *
 *  Created on: Jun 1, 2022
 *      Author: iandu
 */

#include "rx_tast.h"
#include "LCDTask/LCDTask.h"
#include "string.h"

#define BUF_LEN 150

uint32_t buf_len = 150;
uint8_t buf[BUF_LEN];
uint32_t i = 0;
uint32_t msg_len = 0;
uint8_t lcd1[128];
uint8_t lcd2[128];
uint32_t sep = 0;


void receive_data_task(void) {

	while (1) {
//		osKernelLock();
		while (msg_len == 0 && i != BUF_LEN) {
			buf[i] = USER_USART1_Receive();
			if (buf[i] == ',') {
				sep = i;
			}
			if (buf[i] == '\0') {
				msg_len = i + 1;
				i = 0;
				//end msg
				break;
			}
			i++;
		}
//		osKernelUnlock();

		if (msg_len > 0) {
			//Process msg
			strncpy(lcd1, buf, sep);
			strncpy(lcd2, buf + sep + 1, msg_len - sep - 1);
			lcd1[sep] = '\0';
			set_lcd(lcd1, lcd2);

			msg_len = 0;
			sep = 0;
		}
		osDelay(1);
	}
}
