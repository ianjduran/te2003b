/*
 * tx_task.c
 *
 *  Created on: Jun 2, 2022
 *      Author: iandu
 */

#include "tx_task.h"

extern osMessageQueueId_t inputQueueHandle;
void transmit_data_task(void) {

	uint8_t keypadInput;
	while (1) {
		osMessageQueueGet(inputQueueHandle, &keypadInput, NULL, 100);
		if (keypadInput != 0) {
			USER_USART1_Transmit(&keypadInput, sizeof(keypadInput));
			keypadInput = 0;
		}
		osDelay(1);
	}
}
