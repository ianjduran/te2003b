/*
 * LCDTask.c
 *
 *  Created on: Jun 2, 2022
 *      Author: abiel
 */

#include "LCDTask/LCDTask.h"
#include <string.h>
#include "lcd.h"

osMessageQueueId_t lcdMsgHandle;
const osMessageQueueAttr_t lcdMessageHandleAttrs =
		{ .name = "lcdMessageHandle", };

void init_lcd_task() {
	LCD_Init();
	lcdMsgHandle = osMessageQueueNew(5, sizeof(LCDData), NULL);
//	set_lcd("Hello World Hola Ian Hola abiel Hola andy hola diego", "Buenas buenas buenas buenas!");
	set_lcd("Welcome to our Mp3 Player! Let's start!",
			"Created by Abiel, Ian, Andy and Diego!! ");
}

void lcd_task() {
	init_lcd_task();

	LCDData data;
	memset(data.line1, 0, sizeof(data.line1));
	memset(data.line2, 0, sizeof(data.line2));
	int line1_char = 0;
	int line2_char = 0;
	int line1_delay_count = 0;
	int line2_delay_count = 0;

	while (1) {
		osStatus_t ret = osMessageQueueGet(lcdMsgHandle, &data, NULL, 0);
		if (ret == osOK) {
			//New message, reset current char
			line1_char = 0;
			line2_char = 0;
			line1_delay_count = 1000000;
			line2_delay_count = 1000000;
			LCD_Clear();
		}

		size_t line1_size = strlen(data.line1);
		size_t line2_size = strlen(data.line2);

		//Clear lcd
		//LCD_Clear();
		//LCD_BarGraphic(0,0);

		line1_delay_count++;
		line2_delay_count++;

		if (line1_delay_count >= 250) {
			//Draw line 1
			LCD_Set_Cursor(1, 0);
			for (int i = line1_char; i < line1_char + 16 && i < line1_size;
					i++) {
				//Send chars
				LCD_Put_Char(data.line1[i]);
			}

			//Increment char counts
			line1_char++;
			if (line1_char + 16 > line1_size) {
				line1_char = 0;
			}

			line1_delay_count = 0;
		}

		if (line2_delay_count >= 500) {
			//Draw line 2
			LCD_Set_Cursor(2, 0);
			for (int i = line2_char; i < line2_char + 16 && i < line2_size;
					i++) {
				//Send chars
				LCD_Put_Char(data.line2[i]);
			}

			line2_char++;
			if (line2_char + 16 >= line2_size) {
				line2_char = 0;
			}

			line2_delay_count = 0;
		}

		osDelay(1);
	}
}

osStatus_t set_lcd(char *line1, char *line2) {
	LCDData data;
	strncpy(data.line1, line1, sizeof(data.line1));
	strncpy(data.line2, line2, sizeof(data.line2));
	return osMessageQueuePut(lcdMsgHandle, &data, NULL, 0);
}
