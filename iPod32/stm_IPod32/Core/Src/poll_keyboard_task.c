/*
 * poll_keyboard_task.c
 *
 *  Created on: May 30, 2022
 *      Author: Ian Duran
 */

#include "poll_keyboard_task.h"
#include "LCDTask/LCDTask.h"

extern osMessageQueueId_t inputQueueHandle;

uint8_t pollButton(uint8_t row, uint8_t col) {
	//Set all row pins to 1, except polling row
	for (int i = 4; i < 8; i++) {
		GPIOA->ODR |= 1 << i;
	}
	GPIOA->ODR &= ~(1 << row);

	//Read col
	//return 0;
	return GPIOA->IDR & (1 << col);
}

void poll_keyboard_button_task(void) {
	uint8_t keypad_chars[] = { '1', '4', '7', '*', '2', '5', '8', '0', '3', '6',
			'9', '#', 'A', 'B', 'C', 'D' };
	char data[25];
	int index;
	while (1) {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				int out = pollButton(j + 4, i);
				index = i * 4 + j;
				if (out == 0) {
					while (pollButton(j + 4, i) == 0) {}
					snprintf(data, sizeof(data), "%c", keypad_chars[index]);
//					  set_lcd(data, "WAS PRESSED WUUWUWUWUWUWUWUWUUWUWUWUWUWUWUUWUWUWUW");
					osMessageQueuePut(inputQueueHandle, &(data), 0U, 100);

				}
			}
		}
		osDelay(2);
	}

	// Read col
}
